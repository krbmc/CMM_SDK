#ifndef __CERTIFICATE_HPP__
#define __CERTIFICATE_HPP__

#include "stdafx.hpp"
#include "resource.hpp"

#define CERTIFICATE_KEY_LENGTH 1024
#define CERTIFICATE_VALID_DAYS 365
#define TMP_KEY_FILE "/conf/ssl/tmp/cert.key"
#define TMP_PUBKEY_FILE "/conf/ssl/tmp/cert_public.key"
#define TMP_CNF_FILE "/conf/ssl/tmp/cert.cnf"
#define TMP_CSR_FILE "/conf/ssl/tmp/cert.csr"
#define TMP_CRT_FILE "/conf/ssl/tmp/cert.crt"

#define SERVER_CERTIFICATE_CHAIN_PATH "/conf/ssl/rootca.crt"
#define SERVER_PRIVATE_KEY_PATH "/conf/ssl/rootca.key"
#define SERVER_TMP_DH_PATH "/conf/ssl/dh2048.pem"


// CertificateService Actions
json::value generateCSR(json::value _body);
bool replaceCertificate(json::value _body);
json::value certificateRekey(json::value body, Certificate *certificate);
json::value certificateRenew(Certificate *certificate);


//
Collection* generate_certificate_collection(string _col_odata);//, Collection *_cert_collection);
void generate_ssl_private_key(fs::path _key_filename, string _key_length);
void generate_ssl_public_key(fs::path _private, fs::path _public, string _outform);
void generate_ssl_config_file(fs::path _config_filename, fs::path _key_filename, CertContent _info);
json::value generate_CSR_return_result(fs::path _conf, fs::path _key, fs::path _csr, string _target_id);
void generate_certificate(fs::path _csr, fs::path _key, fs::path _crt, string _valid_days);
string generate_certificate_resource(fs::path _crt, Collection *_cert_collection);
void update_certificate_resource(fs::path _crt, Certificate *_certificate);

void apply_https_ssl_file(string odata);

// certificate에서 정보 읽어내는 함수들
string get_issuer_string(fs::path crt);
string get_subject_string(fs::path crt);
string get_startdate_string(fs::path crt);
string get_enddate_string(fs::path crt);
string get_email_string(fs::path crt);
string get_cert_string(fs::path crt);
string get_key_length_string(fs::path crt);

bool parse_CertContent_string(string _info, CertContent &_content);
void insert_CertContent_to_resource(Certificate *_certificate, CertContent _content, string _what);
string parse_validate_info_string(string _info);
string parse_key_length_string(string info);

string file2str(string _file_path);

// 제너레이트한 CSR가지고 Cert파일 만드는거 함수 구현하고
// 그런 다음에 Certservice 의 ReplaceCertificate 함수 구현
// update_cert_with_pem 참고해서 cert파일 만들었을때 certificate리소스 생성하는함수 만들고
// update_cert_with_pem을 변경함수로 수정하기

#endif
