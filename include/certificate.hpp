#ifndef __CERTIFICATE_HPP__
#define __CERTIFICATE_HPP__

#include "stdafx.hpp"
#include "resource.hpp"

#define CERTIFICATE_KEY_LENGTH 1024
#define CERTIFICATE_VALID_DAYS 365
#define TMP_KEY_FILE "/conf/ssl/tmp/cert.key"
#define TMP_CNF_FILE "/conf/ssl/tmp/cert.cnf"
#define TMP_CSR_FILE "/conf/ssl/tmp/cert.csr"
#define TMP_CRT_FILE "/conf/ssl/tmp/cert.crt"


// CertificateService Actions
json::value generateCSR(json::value _body);


//
void generate_ssl_private_key(fs::path _key_filename, string _key_length);
void generate_ssl_config_file(fs::path _config_filename, fs::path _key_filename, CertContent _info);
json::value generate_CSR_return_result(fs::path _conf, fs::path _key, fs::path _csr, string _target_id);
void generate_certificate(fs::path _csr, fs::path _key);
// void generate_certificate_resource
string file2str(string _file_path);

// 제너레이트한 CSR가지고 Cert파일 만드는거 함수 구현하고
// 그런 다음에 Certservice 의 ReplaceCertificate 함수 구현
// update_cert_with_pem 참고해서 cert파일 만들었을때 certificate리소스 생성하는함수 만들고
// update_cert_with_pem을 변경함수로 수정하기

#endif
