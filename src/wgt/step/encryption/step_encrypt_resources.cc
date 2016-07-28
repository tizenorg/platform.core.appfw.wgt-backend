// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/encryption/step_encrypt_resources.h"

#include <web_app_enc.h>

#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>

#include <common/utils/file_util.h>

#include <manifest_parser/utils/logging.h>
#include <sys/stat.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <string>

namespace {

const std::size_t ENCRYPTION_CHUNK_MAX_SIZE = 8192; // bytes
const std::set<std::string> encryptSet { ".html", ".htm", ".css", ".js"};

FILE* openFile(const std::string& path, const std::string& mode)
{
  FILE* result = NULL;

  do {
    result = fopen(path.c_str(), mode.c_str());
  } while ((NULL == result));

  return result;
}

std::size_t readBytes(unsigned char* buffer, std::size_t count, FILE* stream)
{
  std::size_t result = std::fread(buffer,
                                  sizeof(unsigned char),
                                  count,
                                  stream);
  if (result != count) {
    if (0 != std::ferror(stream)) {
      LOG(ERROR) << "Error while reading data";
    }
  }

  return result;
}

void writeBytes(unsigned char* buffer, std::size_t count, FILE* stream)
{
  // original file is treated as destination!
  std::size_t bytesWritten = 0;
  std::size_t bytesToWrite = 0;
  do {
    bytesToWrite = count - bytesWritten;
    bytesWritten = std::fwrite(buffer + bytesWritten,
                               sizeof(unsigned char),
                               count - bytesWritten,
                               stream);
    if ((bytesWritten != bytesToWrite)) {
      LOG(ERROR) << "Error while writing data";
      free(buffer);
      fclose(stream);
    }
  } while ((bytesWritten != bytesToWrite));
}

}  // namespace


namespace wgt {
namespace encryption {

namespace bf = boost::filesystem;
namespace bs = boost::system;

common_installer::Step::Status StepEncryptResources::precheck() {
  backend_data_ = static_cast<WgtBackendData*>(context_->backend_data.get());
  if (!backend_data_) {
    LOG(ERROR) << "no backend data";
    return common_installer::Step::Status::ERROR;
  }

  SetEncryptionRoot();

  if (input_.empty()) {
    LOG(ERROR) << "unpacked_dir_path attribute is empty";
    return Step::Status::INVALID_VALUE;
  }
  if (!bf::exists(input_)) {
    LOG(ERROR) << "unpacked_dir_path (" << input_ << ") path does not exist";
    return Step::Status::INVALID_VALUE;
  }

  return common_installer::Step::Status::OK;
}

common_installer::Step::Status StepEncryptResources::process() {
  if (!backend_data_->settings.get().encryption_enabled()) {
    LOG(DEBUG) << "no encryption";
    return common_installer::Step::Status::OK;
  }
  LOG(DEBUG) << "Encrypting";

  if (!Encrypt(input_)) {
    LOG(ERROR) << "Error during encryption";
    return common_installer::Step::Status::ERROR;
  }

  return common_installer::Step::Status::OK;
}

bool StepEncryptResources::Encrypt(const bf::path &src) {
  // traversing through src dir (recurrence if subdir found)
  // for every file found, check if it should be encrypted (ToBeEncrypted)
  // if yes, encrypt it (and replace original one)
  // if not, leave it
  for (bf::directory_iterator file(src);
       file != bf::directory_iterator();
       ++file) {
    bs::error_code error_code;
    bf::path current(file->path());

    bool is_dir = bf::is_directory(current, error_code);
    if (error_code)
      return false;
    if (is_dir) {
      if (!Encrypt(current))
        return false;
      continue;
    }

    bool is_sym = bf::is_symlink(current, error_code);
    if (error_code)
      return false;
    if (is_sym)
      continue;  // TODO(p.sikorski) is it enough?

    // it is regular file (not dir, not symlink)
    if (ToBeEncrypted(current)) {
      LOG(INFO) << "File for encryption: " << current;
      if (!EncryptFile(current))
        return false;
    }
  }
  return true;
}

bool StepEncryptResources::EncryptFile(const bf::path &src) {
  std::string encFile = src.string() + ".enc";

  struct stat info;
  memset(&info, 0, sizeof(info));
  if (stat(src.string().c_str(), &info) != 0) {
    LOG(ERROR) << "Could not access file " << src.string();
    return false;
  }
  const std::size_t fileSize = info.st_size;
  if (0 == fileSize) {
    LOG(ERROR) << src.string().c_str() << " size is 0, so encryption is skiped";
    return true;
  }

  FILE *input = openFile(src.string().c_str(), "rb");
  if (input == NULL) {
    LOG(ERROR) << "Cannot open file for encryption: " << src.string();
    return false;
  }

  FILE *output = openFile(encFile, "wb");
  if (output == NULL) {
    LOG(ERROR) << "Cannot create encrypted file: " << encFile;
    return false;
  }

  std::size_t chunkSize = (fileSize > ENCRYPTION_CHUNK_MAX_SIZE
                                 ? ENCRYPTION_CHUNK_MAX_SIZE : fileSize);

  std::unique_ptr<unsigned char[]> inChunk(new unsigned char[chunkSize]);
  std::size_t bytesRead = 0;

  do {
    bytesRead = readBytes(inChunk.get(), chunkSize, input);
    if (0 != bytesRead) {
      unsigned char* encrypted_data = nullptr;
      size_t encrypted_size = 0;
      // TODO(p.sikorski) check if it is Preloaded
      int ret;
      if (context_->request_mode.get() == common_installer::RequestMode::GLOBAL) {
        ret = wae_encrypt_global_web_application(
                context_->pkgid.get().c_str(),
                context_->is_preload_request.get() ?
                true : false,
                inChunk.get(),
                (size_t)bytesRead,
                &encrypted_data,
                &encrypted_size);
      } else {
          ret = wae_encrypt_web_application(
                  context_->uid.get(),
                  context_->pkgid.get().c_str(),
                  inChunk.get(),
                  (size_t)bytesRead,
                  &encrypted_data,
                  &encrypted_size);
      }

      if (WAE_ERROR_NONE != ret) {
        LOG(ERROR) << "Error during encrypting:";
        switch (ret) {
          case WAE_ERROR_INVALID_PARAMETER:
            LOG(ERROR) << "WAE_ERROR_INVALID_PARAMETER";
            break;
          case WAE_ERROR_PERMISSION_DENIED:
            LOG(ERROR) << "WAE_ERROR_PERMISSION_DENIED";
            break;
          case WAE_ERROR_NO_KEY:
            LOG(ERROR) << "WAE_ERROR_NO_KEY";
            break;
          case WAE_ERROR_KEY_MANAGER:
            LOG(ERROR) << "WAE_ERROR_KEY_MANAGER";
            break;
          case WAE_ERROR_CRYPTO:
            LOG(ERROR) << "WAE_ERROR_CRYPTO";
            break;
          case WAE_ERROR_UNKNOWN:
            LOG(ERROR) << "WAE_ERROR_UNKNOWN";
            break;
          default:
            LOG(ERROR) << "UNKNOWN";
            break;
        }
        fclose(output);
        fclose(input);
        return false;
      }

      if (encrypted_size <= 0) {
        LOG(ERROR) << "Encryption Failed using TrustZone";
        fclose(output);
        fclose(input);
        return false;
      }

      std::stringstream toString;
      toString << encrypted_size;

      writeBytes((unsigned char*)toString.str().c_str(), sizeof(int), output);
      writeBytes((unsigned char*)encrypted_data, encrypted_size, output);
      free(encrypted_data);
    }
    inChunk.reset(new unsigned char[chunkSize]);

  } while (0 == std::feof(input));

  fclose(output);
  fclose(input);

  LOG(DEBUG) << "File encrypted successfully";
  if (0 != unlink(src.string().c_str())) {
    return false;
  }

  LOG(DEBUG) << "Rename encrypted file";
  if (0 != std::rename(encFile.c_str(), src.string().c_str())) {
    return false;
  }

  return true;
}

void StepEncryptResources::SetEncryptionRoot() {
  input_ = context_->unpacked_dir_path.get();
}

bool StepEncryptResources::ToBeEncrypted(const bf::path &file) {
  size_t found_key = file.string().rfind(".");
  if (std::string::npos != found_key) {
    std::string mimeType = file.string().substr(found_key);
    std::transform(mimeType.begin(), mimeType.end(), mimeType.begin(),
                   ::tolower);
    return encryptSet.count(mimeType) > 0;
  }
  return false;
}

}  // namespace encryption
}  // namespace wgt
