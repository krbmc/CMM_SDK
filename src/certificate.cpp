#include "certificate.hpp"


/**
 * @brief CertificateService.GenerateCSR Action 수행함수
 *  CSR(Certificate Signing Request) 를 생성한다.
 * 
 * @param _body CSR을 Generate하는 데에 필요한 정보가 담긴 json
 */
json::value generateCSR(json::value _body)
{
    string cmds;
    json::value rsp;
    Collection *cert_collection; // certification collection

    // required
    CertContent info;
    string certificate_odata_id;
    
    // string country;
    // string city;
    // string common_name;
    // string state;
    // string organization;
    // string organizational_unit;

    // optional (not implemented..)
    // vector<string> alternative_names;
    // vector<string> key_usage;
    // string challenge_password; // pass phrase
    // string contact_person;
    // string email;
    // string given_name;
    // string initials;
    // string key_curve_id; // ahffk
    // string key_pair_algorithm; // rsa only!
    // string surname;
    // string unstructured_name;


    log(trace) << "[...]GenerateCSR start";
    // Certificate 저장될 리소스의 Collection 을 찾거나 생성하는 부분
    try
    {
        /* code */
        certificate_odata_id = _body.at("CertificateCollection").at("@odata.id").as_string();

        if((cert_collection = generate_certificate_collection(certificate_odata_id)) == nullptr)
        // if(!generate_certificate_collection(certificate_odata_id, cert_collection))
            return json::value::null();
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        log(error) << "[In Generate CSR] : Certificate Collection Extract Error";
        return json::value::null();
    }
    
    // #1 required 처리하고 -- o
    // #2 우선 임시 폴더 tmp 하나 경로 정해서 거기다가 fs 지정하고
    // #3 openssl관련 crt생성한다음에
    // #4 certificate 리소스 생성하고 해당 디렉토리 밑에 파일 옮기기
    // certificatecollection 에 해당 cert가 들어갈 컬렉션 경로가 들어올건데
    // 이게 들어갈수있는게 일단은 networkprotocol에 https로 들어오는거랑 account에 들어오는거임 그거 외엔
    // 무시해줘야함
    // 그렇게 하고 패스랑 키 생성은 임시 폴더에다가 만들거야 만들고나서 그걸 나중에 컬렉션 생성하고 그 폴더에다가
    // 넣는식으로 하자
    if(!get_value_from_json_key(_body, "Country", info.country))
    {
        log(error) << "[In Generate CSR] : Country Field is Required";
        return json::value::null();
    }

    // Country는 두글자 영문대문자코드인지 검사필요함
    if(!validateCountryCode(info.country))
    {
        log(error) << "[In Generate CSR] : Country Field is Invalid";
        return json::value::null();
    }

    if(!get_value_from_json_key(_body, "City", info.city))
    {
        log(error) << "[In Generate CSR] : City Field is Required";
        return json::value::null();
    }

    if(!get_value_from_json_key(_body, "CommonName", info.commonName))
    {
        log(error) << "[In Generate CSR] : CommonName Field is Required";
        return json::value::null();
    }

    if(!get_value_from_json_key(_body, "State", info.state))
    {
        log(error) << "[In Generate CSR] : State Field is Required";
        return json::value::null();
    }

    if(!get_value_from_json_key(_body, "Organization", info.organization))
    {
        log(error) << "[In Generate CSR] : Organization Field is Required";
        return json::value::null();
    }

    if(!get_value_from_json_key(_body, "OrganizationalUnit", info.organizationalUnit))
    {
        log(error) << "[In Generate CSR] : OrganizationalUnit Field is Required";
        return json::value::null();
    }

    get_value_from_json_key(_body, "Email", info.email);
    
    // #1 key, conf(inform), csr path declare 
    fs::path key(TMP_KEY_FILE);
    fs::path pubkey(TMP_PUBKEY_FILE);
    fs::path conf(TMP_CNF_FILE);
    fs::path csr(TMP_CSR_FILE);
    fs::path cert(TMP_CRT_FILE);
    // fs::path key("/conf/ssl/tmp/cert.key");
    // fs::path conf("/conf/ssl/tmp/cert.cnf");
    // fs::path csr("/conf/ssl/tmp/cert.csr");
    // fs::path key("/conf/ssl/cert.key");
    // fs::path conf("/conf/ssl/cert.cnf");
    // fs::path csr("/conf/ssl/cert.csr");
    // string key_length = "1024";

    // #2 이미 존재한다면, 삭제
    remove_if_exists(conf);
    remove_if_exists(key);
    remove_if_exists(pubkey);
    remove_if_exists(csr);
    remove_if_exists(cert);
    
    // #3 ssl key (개인키) 및 공개키 생성
    log(trace) << "[...]Generate SSL Private Key";
    generate_ssl_private_key(key, to_string(CERTIFICATE_KEY_LENGTH));
    // generate_ssl_public_key(key, pubkey, "PEM");
    
    // #4 ssl config file 생성
    log(trace) << "[...]Generate SSL Config File";
    generate_ssl_config_file(conf, key, info);

    // #5 CSR 생성 && return request
    log(trace) << "[...]Generate CSR";
    rsp = generate_CSR_return_result(conf, key, csr, certificate_odata_id);

    // 스키마상에는 CSR 생성까지만인데 CERT를 만드는 액션이 따로 없어서
    // 임의로 이부분에 구현해놓음
    generate_certificate(csr, key, cert, to_string(CERTIFICATE_VALID_DAYS));
    
    string cert_id;
    cert_id = generate_certificate_resource(cert, cert_collection);

    if(!fs::exists(cert_id))
    {
        string cmd = "mkdir " + cert_id;
        system(cmd.c_str());
    }

    string mv_cmd = "mv ";
    mv_cmd.append(TMP_KEY_FILE).append(" ")//.append(TMP_PUBKEY_FILE).append(" ")
    .append(TMP_CNF_FILE).append(" ").append(TMP_CSR_FILE).append(" ")
    .append(TMP_CRT_FILE).append(" ").append(cert_id);
    system(mv_cmd.c_str());

    apply_https_ssl_file(cert_id);
    

    return rsp;
}

bool replaceCertificate(json::value _body)
{
    string target_odata_id;
    Certificate *target_certificate;
    string body_certificateString, body_certificateType;
    
    /////////// -------------
    // #1 body에서 CSR string 및 CSR Type, target Uri 추출하기
    log(info) << "[...]Replace Certificate Start";
    try
    {
        /* code */
        target_odata_id = _body.at("CertificateUri").at("@odata.id").as_string();
        if(record_is_exist(target_odata_id))
        {
            uint8_t r_type = g_record[target_odata_id]->type;
            if(r_type != CERTIFICATE_TYPE)
            {
                log(warning) << "[In Replace Certificate] : Target Certificate Uri is not a Certificate";
                return false;
            }
        }
        else
        {
            log(warning) << "[In Replace Certificate] : Target Certificate Uri is not exist";
            return false;
        }

        target_certificate = (Certificate *)g_record[target_odata_id];

        if(!get_value_from_json_key(_body, "CertificateString", body_certificateString))
        {
            log(warning) << "[In Replace Certificate] : CertificateString Field is Required";
            return false;
        }

        if(!get_value_from_json_key(_body, "CertificateType", body_certificateType))
        {
            log(warning) << "[In Replace Certificate] : CertificateType Field is Required";
            return false;
        }


        // if(get_value_from_json_key(body, "CertificateString", replacedCert->certificateString))
        // get_value_from_json_key(body, "CertificateType", replacedCert->certificateType);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        log(warning) << "[In Replace Certificate] : Target Certificate Uri Extract Error";
        return false;
    }
    
    
    // Certificate *replacedCert = (Certificate *)g_record[target_odata_id];

    
    
    // #2 crt 파일 입력받은 certificate로 교체
    // 존재하면 백업 후 교체. 존재하지 않는다면 생성.
    fs::path cert_file(target_odata_id + "/cert.crt");
    fs::path cert_bk_file(target_odata_id + "/cert_old.crt");
    // fs::path cert_file(target_odata_id + ".pem");
    if (fs::exists(cert_file)){
        // backup 필요??
        fs::copy(cert_file, cert_bk_file);
        fs::remove(cert_file);
        log(info) << "[...]Original Certificate file is backup..";
    }
    
    ofstream cert(cert_file.string());
    cert << body_certificateString;
    cert.close();
    
    // #3 수정된 certificate 정보 읽어서 g_record 수정
    update_certificate_resource(cert_file, target_certificate);
    target_certificate->certificateType = body_certificateType;
    // target_certificate->certificateString = body_certificateString;
    // update_cert_with_pem(cert_file, replacedCert);
    log(info) << "[...]Certificate is replaced..";
    resource_save_json(target_certificate);
    // resource_save_json(replacedCert);
    
    return true;
    
}

json::value certificateRekey(json::value body, Certificate *certificate)
{
    // body에서는 일단 key bit length정도 사용하고 나머진 일단 미구현 key bit length optional이고..
    // 그리고 fs::path를 기존에는 conf껄 사용했는데 이건 cert꺼 사용해야됨
    // key 새로 제너레이트 하면서 기존키 old로 바꾸고 그걸로 컨픽그대로 csr다시발급 기존꺼 old화
    // 그거로 다시 crt 다시 만들고 리소스 업데이트하고 기존csr old화하고

    json::value rsp;
    int key_bit_length;
    fs::path key(certificate->odata.id + "/cert.key");
    fs::path key_old(certificate->odata.id + "/cert_old.key");
    fs::path conf(certificate->odata.id + "/cert.cnf");
    fs::path conf_old(certificate->odata.id + "/cert_old.cnf");
    fs::path csr(certificate->odata.id + "/cert.csr");
    fs::path csr_old(certificate->odata.id + "/cert_old.csr");
    fs::path crt(certificate->odata.id + "/cert.crt");
    fs::path crt_old(certificate->odata.id + "/cert_old.crt");

    // not implemented..
    string key_curve_id;
    string key_pair_algorithm;
    string challenge_password;

    // #1 body data 추출
    if(!(get_value_from_json_key(body, "KeyBitLength", key_bit_length)))
        key_bit_length = CERTIFICATE_KEY_LENGTH;

    // #2 기존 cert관련 파일들 old백업
    if(fs::exists(key))
    {
        fs::copy(key, key_old, fs::copy_options::overwrite_existing);
        fs::remove(key);
    }
    if(fs::exists(conf))
    {
        fs::copy(conf, conf_old, fs::copy_options::overwrite_existing);
        fs::remove(conf);
    }
    if(fs::exists(csr))
    {
        fs::copy(csr, csr_old, fs::copy_options::overwrite_existing);
        fs::remove(csr);
    }
    if(fs::exists(crt))
    {
        fs::copy(crt, crt_old, fs::copy_options::overwrite_existing);
        fs::remove(crt);
    }
    log(info) << "[...]Original Certificate file is backup..";

    // #3 기존 Certificate에서 CertContent 정보 추출
    CertContent origin_info = certificate->subject;
    origin_info.email = certificate->email;

    // #4 key -> cnf -> csr -> crt 순서의 Generate
    generate_ssl_private_key(key, to_string(key_bit_length));
    generate_ssl_config_file(conf, key, origin_info);
    rsp = generate_CSR_return_result(conf, key, csr, certificate->odata.id);
    generate_certificate(csr, key, crt, to_string(CERTIFICATE_VALID_DAYS));

    update_certificate_resource(crt, certificate);

    return rsp;
}

json::value certificateRenew(Certificate *certificate)
{
    // 현재는 body없지만 optional로 ChallengePassword 들어올 수 있음...

    json::value rsp;
    int key_bit_length;
    fs::path key(certificate->odata.id + "/cert.key");
    fs::path key_old(certificate->odata.id + "/cert_old.key");
    fs::path conf(certificate->odata.id + "/cert.cnf");
    fs::path conf_old(certificate->odata.id + "/cert_old.cnf");
    fs::path csr(certificate->odata.id + "/cert.csr");
    fs::path csr_old(certificate->odata.id + "/cert_old.csr");
    fs::path crt(certificate->odata.id + "/cert.crt");
    fs::path crt_old(certificate->odata.id + "/cert_old.crt");

    // #2 기존 cert관련 파일들 old백업
    if(!fs::exists(key))
    {
        log(warning) << "[In Certificate Renew] : ssl key file doesn't exists.. failed to renew";
        return json::value::null();
    }

    if(fs::exists(conf))
    {
        fs::copy(conf, conf_old, fs::copy_options::overwrite_existing);
        fs::remove(conf);
    }
    if(fs::exists(csr))
    {
        fs::copy(csr, csr_old, fs::copy_options::overwrite_existing);
        fs::remove(csr);
    }
    if(fs::exists(crt))
    {
        fs::copy(crt, crt_old, fs::copy_options::overwrite_existing);
        fs::remove(crt);
    }
    log(info) << "[...]Original Certificate file is backup..";

    // #3 기존 Certificate에서 CertContent 정보 추출
    CertContent origin_info = certificate->subject;
    origin_info.email = certificate->email;

    generate_ssl_config_file(conf, key, origin_info);
    rsp = generate_CSR_return_result(conf, key, csr, certificate->odata.id);
    generate_certificate(csr, key, crt, to_string(CERTIFICATE_VALID_DAYS));

    update_certificate_resource(crt, certificate);

    return rsp;
}

/**
 * @brief Body로 들어온 Certificate Collection의 존재 검사 및 생성하는 함수
 * 
 * @param _col_odata Collection Odata id
 * @return Certificate Collection 포인터 반환
 */
Collection* generate_certificate_collection(string _col_odata)//, Collection *_cert_collection)
{

    Collection *cert_collection;
    // /redfish/v1/Managers/BMC/NetworkProtocol/HTTPS/Certificates
    // /redfish/v1/AccountService/Accounts/1/Certificates
    if(record_is_exist(_col_odata))
    {
        // Certificate Collection 존재하는 경우
        uint8_t resource_type = (g_record[_col_odata])->type;
        if(resource_type == COLLECTION_TYPE)
            cert_collection = (Collection *)g_record[_col_odata];
        else
        {
            log(error) << "[In Generate CSR] : CertificateCollection is exist But It is not Collection Type";
            return nullptr;
            // return false;
            // return json::value::null();
        }
    }
    else
    {
        // Certificate Collection 존재하지 않는 경우
        // 컬렉션을 연결하려는 리소스가 존재하는지 부터 확인해야함
        // HTTPS 인지 확인하기위해 last필요(HTTPS는 리소스로 없고 이름만 있는거라)
        string last = get_current_object_name(get_parent_object_uri(_col_odata, "/"), "/");
        string owner_resource;

        if(last == "HTTPS")
            owner_resource = get_parent_object_uri(get_parent_object_uri(_col_odata, "/"), "/");
        else
            owner_resource = get_parent_object_uri(_col_odata, "/");
        
        if(!record_is_exist(owner_resource))
        {
            log(error) << "[In Generate CSR] : Certificate Collection Owner Resource is not exist";
            return nullptr;
            // return false;
            // return json::value::null();
        }

        cert_collection = new Collection(_col_odata, ODATA_CERTIFICATE_COLLECTION_TYPE);
        uint8_t resource_type = (g_record[owner_resource])->type;
        if(resource_type == ACCOUNT_TYPE)
        {
            Account *account = (Account *)g_record[owner_resource];
            account->certificates = cert_collection;
        }
        else if(resource_type == NETWORK_PROTOCOL_TYPE)
        {
            NetworkProtocol *network = (NetworkProtocol *)g_record[owner_resource];
            network->certificates = cert_collection;
        }
        else
        {
            log(error) << "[In Generate CSR] : \"" << owner_resource << "\" does not support Certificate Collection";
            return nullptr;
            // return false;
            // return json::value::null();
        }
    }

    return cert_collection;
    // return true;
}


/**
 * @brief Openssl Private Key 생성함수
 * 
 * @param _key_filename key file name
 * @param _key_length key length
 */
void generate_ssl_private_key(fs::path _key_filename, string _key_length)
{
    string cmd = "openssl genrsa -out " + _key_filename.string() + " " + _key_length;
    try
    {
        system(cmd.c_str());
        log(info) << "[###]Generate Private Key SUCCESS";
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        log(error) << "[###]Generate Private Key FAIL";
    }

    return ;
}

/**
 * @brief Openssl Public Key 생성함수
 * 
 * @param _private private key file name
 * @param _public public key file name
 * @param _outform output format (DER or NET or PEM(*))
 */
void generate_ssl_public_key(fs::path _private, fs::path _public, string _outform)
{
    string cmd = "openssl rsa -in " + _private.string() + " -out "
    + _public.string() + " -pubout -outform " + _outform;

    try
    {
        system(cmd.c_str());
        log(info) << "[###]Generate Public Key SUCCESS";
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        log(error) << "[###]Generate Public Key FAIL";
    }

    return ;

}

/**
 * @brief CSR 을 만들기위한 Config파일 생성함수
 * 
 * @param _config_filename config file name
 * @param _key_filename key file name 
 * @param _info config info
 */
void generate_ssl_config_file(fs::path _config_filename, fs::path _key_filename, CertContent _info)
{
    char cert_text[15][200];
    string key_length = to_string(CERTIFICATE_KEY_LENGTH);

    // conf파일에 size가 최소1이상이어야 하는 조건존재
    if(_info.city == "")
        _info.city = " ";
    if(_info.state == "")
        _info.state = " ";
    if(_info.organization == "")
        _info.organization = " ";
    if(_info.organizationalUnit == "")
        _info.organizationalUnit = " ";
    if(_info.commonName == "")
        _info.commonName = " ";
    if(_info.email == "")
        _info.email = " ";

    sprintf(cert_text[0], "[ req ]\n");	
    sprintf(cert_text[1], "default_bits\t= %s\n", key_length.c_str());
    sprintf(cert_text[2], "default_md\t= sha256\n");	
    sprintf(cert_text[3], "default_keyfile\t=%s\n", _key_filename.c_str());
    sprintf(cert_text[4], "prompt\t = no\n");
    sprintf(cert_text[5], "encrypt_key\t= no\n\n");
    sprintf(cert_text[6], "# base request\ndistinguished_name = req_distinguished_name\n");
    sprintf(cert_text[7], "\n# distinguished_name\n[ req_distinguished_name ]\n");	
    sprintf(cert_text[8], "countryName\t= \"%s\"\n", _info.country.c_str());
    sprintf(cert_text[9], "stateOrProvinceName\t= \"%s\"\n", _info.state.c_str());
    sprintf(cert_text[10], "localityName\t=\"%s\"\n", _info.city.c_str());
    sprintf(cert_text[11], "organizationName\t=\"%s\"\n", _info.organization.c_str());	
    sprintf(cert_text[12], "organizationalUnitName\t=\"%s\"\n", _info.organizationalUnit.c_str());	
    sprintf(cert_text[13], "commonName\t=\"%s\"\n", _info.commonName.c_str());
    sprintf(cert_text[14], "emailAddress\t=\"%s\"\n", _info.email.c_str());
    
    ofstream cert_conf(_config_filename);
    for (int i = 0; i < 15; i++)
        cert_conf << cert_text[i];
    cert_conf.close();

    return ;
}

/**
 * @brief CSR 생성함수
 * 
 * @param _conf config file
 * @param _key key file
 * @param _csr csr file
 * @param _target_id Certificate가 생성될 Collection odata
 */
json::value generate_CSR_return_result(fs::path _conf, fs::path _key, fs::path _csr, string _target_id)
{
    json::value rsp;
    string cmds = "openssl req -config " + _conf.string() + " -new -key " + _key.string() + " -out " + _csr.string() + " -verbose";
    
    if (!std::system(cmds.c_str())){
        if (fs::exists(_csr) && fs::exists(_key) && fs::exists(_conf)){
            log(info) << "[###]CSR is generated";
        
            json::value odata_id;
            
            int r_type = g_record[_target_id]->type;
            odata_id["@odata.id"] = json::value::string(_target_id);

            if(r_type == CERTIFICATE_TYPE)
                rsp["Certificate"] = odata_id;
            else if(r_type == COLLECTION_TYPE)
                rsp["CertificateCollection"] = odata_id;
            
            rsp["CSRString"] = json::value::string(file2str(_csr.string()));
        }
    }else{
        log(warning) << "[Error] Failed to Generate CSR";
        json::value msg;
        msg["Message"] = json::value::string("Request Failed.");
        rsp["Failed"] = msg;
    }

    return rsp;
}

void generate_certificate(fs::path _csr, fs::path _key, fs::path _crt, string _valid_days)
{
    log(info) << "[...]Generate Certificate";
    string cmd = "openssl req -x509 -in " + _csr.string() + " -key " + _key.string()
    + " -out " + _crt.string() + " -days " + _valid_days;
    // + " -out " + TMP_CRT_FILE + " -days " + to_string(CERTIFICATE_VALID_DAYS);

    try
    {
        /* code */
        system(cmd.c_str());
        log(info) << "[###]Generate Certificate SUCCESS";

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        log(error) << "[###]Generate Certificate FAIL";
    }
    
    return ;
}

/**
 * @brief certificate를 가지고 certificate 리소스를 만드는 함수
 * 
 * @param _crt certificate 파일
 * @param _cert_collection 리소스가 add될 certificate collection포인터
 * @return string : 만들어진 certificate 리소스의 odata.id를 반환
 */
string generate_certificate_resource(fs::path _crt, Collection *_cert_collection)
{
    // Certificate Resource 1개 생성해서
    // CertificateString, Issuer, Subject, Validnotbefore,after
    // key usage??
    // Collection에 add_member 아맞다 컬렉션만드는거에 save_resourec안했네 - 여기서 같이함
    string issuer_info, subject_info, valid_start, valid_end, email, cert_string, key_bit_length;
    CertContent issuer_content, subject_content;
    
    string odata_id = _cert_collection->odata.id;
    odata_id.append("/").append(to_string((_cert_collection->members.size())+1));
    Certificate *certificate = new Certificate(odata_id);

    issuer_info = get_issuer_string(_crt);
    // log(trace) << "[!@#$ ISSUER] ------------------";
    // log(trace) << "CMD : " << cmd_issuer;
    // log(trace) << "INFO : " << issuer_info;
    parse_CertContent_string(issuer_info, issuer_content);
    insert_CertContent_to_resource(certificate, issuer_content, "Issuer");
    
    subject_info = get_subject_string(_crt);
    // log(trace) << "[!@#$ SUBJECT] ------------------";
    // log(trace) << "CMD : " << cmd_subject;
    // log(trace) << "INFO : " << subject_info;
    parse_CertContent_string(subject_info, subject_content);
    insert_CertContent_to_resource(certificate, subject_content, "Subject");

    valid_start = get_startdate_string(_crt);
    // log(trace) << "[!@#$ VALID START] ------------------";
    // log(trace) << "CMD : " << cmd_start;
    // log(trace) << "INFO : " << valid_start;
    certificate->validNotBefore = parse_validate_info_string(valid_start);

    valid_end = get_enddate_string(_crt);
    // log(trace) << "[!@#$ VALID END] ------------------";
    // log(trace) << "CMD : " << cmd_end;
    // log(trace) << "INFO : " << valid_end;
    certificate->validNotAfter = parse_validate_info_string(valid_end);

    email = get_email_string(_crt);
    // log(trace) << "[!@#$ EMAIL] ------------------";
    // log(trace) << "CMD : " << cmd_email;
    // log(trace) << "INFO : " << email;
    certificate->email = email;

    cert_string = get_cert_string(_crt);
    // log(trace) << "[!@#$ CERTIFICATE STRING] ------------------";
    // log(trace) << "CMD : " << cmd_certstring;
    // log(trace) << "INFO : " << cert_string;

    key_bit_length = get_key_length_string(_crt);
    certificate->key_bit_length = improved_stoi(parse_key_length_string(key_bit_length));

    certificate->certificateString = cert_string;
    certificate->certificateType = "PEM";

    _cert_collection->add_member(certificate);
    CertificateLocation *location = (CertificateLocation *)g_record[ODATA_CERTIFICATE_LOCATION_ID];
    location->certificates.push_back(certificate);

    resource_save_json(_cert_collection);
    resource_save_json(certificate);
    resource_save_json(location);
    
    return certificate->odata.id;
    
}

void update_certificate_resource(fs::path _crt, Certificate *_certificate)
{
    // 여기서는 cert파일이 certstring이 바뀐 이후임 그래서
    // 새로운 _crt에서 정보를 읽어다가 새로이 _certificate에 변경해주면됨

    // issuer, subject, start, end, email, certstring 6개 갱신
    string issuer_info, subject_info, valid_start, valid_end, email, key_bit_length;
    CertContent issuer_content, subject_content;

    // Issuer Update
    issuer_info = get_issuer_string(_crt);
    parse_CertContent_string(issuer_info, issuer_content);
    insert_CertContent_to_resource(_certificate, issuer_content, "Issuer");

    // Subject Update
    subject_info = get_subject_string(_crt);
    parse_CertContent_string(subject_info, subject_content);
    insert_CertContent_to_resource(_certificate, subject_content, "Subject");

    // ValidNotBefore Update
    valid_start = get_startdate_string(_crt);
    _certificate->validNotBefore = parse_validate_info_string(valid_start);

    // ValidNotAfter Update
    valid_end = get_enddate_string(_crt);
    _certificate->validNotAfter = parse_validate_info_string(valid_end);

    // Email Update
    email = get_email_string(_crt);
    _certificate->email = email;

    // CertString Update
    _certificate->certificateString = get_cert_string(_crt);

    // Key Length Update
    key_bit_length = get_key_length_string(_crt);
    _certificate->key_bit_length = improved_stoi(parse_key_length_string(key_bit_length));


    // update함수는 replaceCertificate, Rekey, Renew를 수행하면서 호출하는데 파일갱신을 모두하고
    // 이 함수로 리소스 내용을 변경하는거라 여기서 apply_https_ssl_file하면 세가지 경우 다 적용됨
    apply_https_ssl_file(_certificate->odata.id);
    

    return ;
}

/**
 * @brief odata에 해당하는 Certificate가 HTTPS의 ID 1의 Certificate일 때 해당 Certificate의
 * cert, key 파일로 서버 config 사용하게 하는 함수
 */
void apply_https_ssl_file(string odata)
{
    // /redfish/v1/Managers/[module]/NetworkProtocol/HTTPS/Certificates/1 -- odata가 이거일때만
    // /redfish/v1/AccountService/Accounts/[id]/Certificates/1
    // 리소스를 생성하고 업데이트 하는 곳에서 호출하는거니깐 리소스는 다 있는걸로 나왔을거임
    // 생각해보니 module도 CMM꺼여야하네
    try
    {
        /* code */
        vector<string> part = string_split(odata, '/');
        if(part.size() != 8)
        {
            log(trace) << "[NOT HTTPS] : URI length is not 8";
            return ;
        }

        if(part[5] != "HTTPS")
        {
            log(trace) << "[NOT HTTPS] : 5th position is not HTTPS";
            return ;
        }

        if(part[3] != CMM_ID)
        {
            log(trace) << "[NOT HTTPS] : 3th position is not CMM module ID";
            return ;
        }

        if(part[7] != "1")
        {
            log(trace) << "[NOT HTTPS] : 8th position is not 1(Certificate ID)";
            return ;
        }

        // 통과다 했으면 비로소 파일 old로 백업하고 복사해넣고 굿
        fs::path server_cert(SERVER_CERTIFICATE_CHAIN_PATH);
        fs::path server_key(SERVER_PRIVATE_KEY_PATH);
        // string old_server_cert, old_server_key;
        fs::path old_server_cert("/conf/ssl/rootca_old.crt");
        fs::path old_server_key("/conf/ssl/rootca_old.key");

        if(fs::exists(server_cert))
        {
            fs::copy(server_cert, old_server_cert, fs::copy_options::overwrite_existing);
            fs::remove(server_cert);
        }

        if(fs::exists(server_key))
        {
            fs::copy(server_key, old_server_key, fs::copy_options::overwrite_existing);
            fs::remove(server_key);
        }

        string cmd_cert = "cp ";
        cmd_cert.append(odata).append("/cert.crt").append(" ").append(server_cert.string());
        system(cmd_cert.c_str());

        string cmd_key = "cp ";
        cmd_key.append(odata).append("/cert.key").append(" ").append(server_key.string());
        system(cmd_key.c_str());
        
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        log(warning) << "[APPLYING HTTPS] : something wrong";
        return ;
    }
    
    return ;
}

string get_issuer_string(fs::path crt)
{
    string result;
    string cmd = "openssl x509 -in " + crt.string() + " -issuer -noout";
    result = get_popen_string(cmd);

    return result;
}

string get_subject_string(fs::path crt)
{
    string result;
    string cmd = "openssl x509 -in " + crt.string() + " -subject -noout";
    result = get_popen_string(cmd);

    return result;
}

string get_startdate_string(fs::path crt)
{
    string result;
    string cmd = "openssl x509 -in " + crt.string() + " -startdate -noout";
    result = get_popen_string(cmd);

    return result;
}

string get_enddate_string(fs::path crt)
{
    string result;
    string cmd = "openssl x509 -in " + crt.string() + " -enddate -noout";
    result = get_popen_string(cmd);

    return result;
}

string get_email_string(fs::path crt)
{
    string result;
    string cmd = "openssl x509 -in " + crt.string() + " -email -noout";
    result = get_popen_string(cmd);

    return result;
}

string get_cert_string(fs::path crt)
{
    string result;
    string cmd = "openssl x509 -in " + crt.string();
    result = get_popen_string(cmd);

    return result;
}

string get_key_length_string(fs::path crt)
{
    string result;
    string cmd = "openssl x509 -in " + crt.string() + " -text -noout | grep \"Public-Key\"";
    result = get_popen_string(cmd);

    return result;
}

/**
 * @brief string에서 CertContent info 추출하는 함수
 * 
 * @param _info Certificate의 CertContent 정보가 한 줄의 string으로 담겨옴(_info에)
 * @param _what Issuer, Subject 구분
 * @return CertContent 
 */
bool parse_CertContent_string(string _info, CertContent &_content)
{
    //issuer= /C=KR/ST=State/L=City/O=Organization/OU=OrganizationalUnit/CN=CommonName
    vector<string> block = string_split(_info, '/');
    for(int i=0; i<block.size(); i++)
    {   
        vector<string> component = string_split(block[i], '=');
        if(component.size() != 2)
        {
            log(warning) << "In parse_CertContent_string Why not 2?";
            return false;
        }

        string key=component[0], value=component[1];
        
        if(key == "C")
            _content.country = value;
        else if(key == "ST")
            _content.state = value;
        else if(key == "L")
            _content.city = value;
        else if(key == "O")
            _content.organization = value;
        else if(key == "OU")
            _content.organizationalUnit = value;
        else if(key == "CN")
            _content.commonName = value;
    }

    return true;
}

/**
 * @brief certificate 리소스에 certcontent 정보 값 넣는 함수
 * 
 * @param _certificate certificate 리소스
 * @param _content CertContent 정보
 * @param _what Issuer / Subject 구분
 */
void insert_CertContent_to_resource(Certificate *_certificate, CertContent _content, string _what)
{
    if(_what == "Issuer")
    {
        _certificate->issuer.country = _content.country;
        _certificate->issuer.city = _content.city;
        _certificate->issuer.state = _content.state;
        _certificate->issuer.commonName = _content.commonName;
        _certificate->issuer.organization = _content.organization;
        _certificate->issuer.organizationalUnit = _content.organizationalUnit;
    }
    else if(_what == "Subject")
    {
        _certificate->subject.country = _content.country;
        _certificate->subject.city = _content.city;
        _certificate->subject.state = _content.state;
        _certificate->subject.commonName = _content.commonName;
        _certificate->subject.organization = _content.organization;
        _certificate->subject.organizationalUnit = _content.organizationalUnit;
    }

}


/**
 * @brief certificate valid date string 추출함수
 * 
 * @param _info 
 * @return string 
 */
string parse_validate_info_string(string _info)
{
    // notBefore=Jun  3 05:38:28 2022 GMT
    string result;
    string back = get_current_object_name(_info, "=");
    vector<string> part = string_split(back, ' ');

    // for(int i=0; i<part.size(); i++)
    // {
    //     cout << part[i] << " / ";
    //     // log(trace) << part[i] << " / ";
    // }
    // cout << endl;

    result = part[3];
    result.append("-");

    if(part[0] == "Jan")
        result.append("01-");
    else if(part[0] == "Feb")
        result.append("02-");
    else if(part[0] == "Mar")
        result.append("03-");
    else if(part[0] == "Apr")
        result.append("04-");
    else if(part[0] == "May")
        result.append("05-");
    else if(part[0] == "Jun")
        result.append("06-");
    else if(part[0] == "Jul")
        result.append("07-");
    else if(part[0] == "Aug")
        result.append("08-");
    else if(part[0] == "Sep")
        result.append("09-");
    else if(part[0] == "Oct")
        result.append("10-");
    else if(part[0] == "Nov")
        result.append("11-");
    else if(part[0] == "Dec")
        result.append("12-");

    if(part[1].length() < 2)
        result.append("0");
    
    result.append(part[1]).append(" ").append(part[2]);

    // cout << "RESULT : " << result << endl;

    return result;

}

string parse_key_length_string(string info)
{
    string result;
    string work_str = info.substr(info.find("(")+1);
    // log(trace) << "working str : " << work_str;

    result = work_str.substr(0, work_str.find_first_not_of("0123456789"));

    // log(trace) << "result : " << result;

    return result;
}




/**
 * @brief file을 읽어서 string으로 반환하는 함수 (Get CSRString)
 * 
 * @param _file_path
 * @return CSR String
 */
string file2str(string _file_path)
{
    auto csr_str = ostringstream{};
    ifstream read_csr(_file_path);
    if (!read_csr.is_open()){
        log(warning) << "Could not open the file : " << _file_path;
        return "";
    }
    csr_str << read_csr.rdbuf();
    read_csr.close();

    return csr_str.str();
}