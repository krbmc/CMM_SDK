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

    // required
    CertContent info;
    string certificate_odata_id;
    // string country;
    // string city;
    // string common_name;
    // string state;
    // string organization;
    // string organization_unit;

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


    // log(info) << "[...]GenerateCSR start";
    try
    {
        /* code */
        certificate_odata_id = _body.at("CertificateCollection").at("@odata.id").as_string();
        // 리소스가... /redfish/v1/Managers/BMC/NetworkProtocol/HTTPS/Certificates
        // 딱 여까지만 들어오니깐.. 자체로 리소스 있는지 파악하고 있다면 Collection인지도
        // 봐야함
        // https의경우 저렇고
        // Account의 경우 /redfish/v1/AccountService/Accounts/1/Certificates
        // 이렇게 들어오겠군 Account의 경우 바로 컬렉션만들면되는데
        // NetworkProtocol의 경우 HTTPS의 리소스가없고 바로 컬렉션이라..
        // 컬렉션이름만 추가적으로 HTTPS/Certificates라고 붙는셈임
        // 그러면 리소스 검사는 어쨋든 odata는 동일하니깐 그대로하면되고
        // 있으면 컬렉션 만들어져있는거니까 바로 그 안에 하나 생성하고 만들면되고
        // 없으면 컬렉션을 만들어서 연결시켜줘야함
        if(record_is_exist(certificate_odata_id))
        {
            // Certificate Collection 존재하는 경우
        }
        else
        {
            // Certificate Collection 존재하지 않는 경우
        }
        // 함수화 하고... act에서 오류안났을때 리소스 생성 수행해야할거같음
        // 그러면 인자로 odata랑,, CertContent,,,
        // odata랑 cert파일 path로 안에 내용물 정보 가져올 수 있으면 됨..

        // 여기에 certificate_odata_id 가
        // 앞에서 4 개로 끊었을때 /redfish/v1/Managers/NetworkProtocol/HTTPS/Certificates
        // /redfish/v1/AccountService/Accounts 인가 2개를 체크하셈
        // vector<string> parts = string_split(certificate_odata_id, '/');
        // for(int i=0; i<parts.size(); i++)
        // {

        // }
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

    if(!get_value_from_json_key(_body, "OrganizationalUnit", info.organizationUnit))
    {
        log(error) << "[In Generate CSR] : OrganizationalUnit Field is Required";
        return json::value::null();
    }
    
    // #1 key, conf(inform), csr path declare 
    fs::path key(TMP_KEY_FILE);
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
    
    // #3 ssl key (개인키) 생성
    // log(info) << "[...]Generate SSL Private Key";
    generate_ssl_private_key(key, to_string(CERTIFICATE_KEY_LENGTH));
    
    // #4 ssl config file 생성
    // log(info) << "[...]Generate SSL Config File";
    generate_ssl_config_file(conf, key, info);
    // char cert_text[14][200];

    // sprintf(cert_text[0], "[ req ]\n");	
    // sprintf(cert_text[1], "default_bits\t= %s\n", key_length.c_str());
    // sprintf(cert_text[2], "default_md\t= sha256\n");	
    // sprintf(cert_text[3], "default_keyfile\t=%s\n", key.c_str());
    // sprintf(cert_text[4], "prompt\t = no\n");
    // sprintf(cert_text[5], "encrypt_key\t= no\n\n");
    // sprintf(cert_text[6], "# base request\ndistinguished_name = req_distinguished_name\n");
    // sprintf(cert_text[7], "\n# distinguished_name\n[ req_distinguished_name ]\n");	
    // sprintf(cert_text[8], "countryName\t= \"%s\"\n", country.c_str());
    // sprintf(cert_text[9], "stateOrProvinceName\t= \"%s\"\n", state.c_str());
    // sprintf(cert_text[10], "localityName\t=\"%s\"\n", city.c_str());
    // sprintf(cert_text[11], "organizationName\t=\"%s\"\n", organization.c_str());	
    // sprintf(cert_text[12], "organizationalUnitName\t=\"%s\"\n", organization_unit.c_str());	
    // sprintf(cert_text[13], "commonName\t=\"%s\"\n", common_name.c_str());
    
    // ofstream cert_conf(conf);
    // for (int i = 0; i < 14; i++)
    //     cert_conf << cert_text[i];
    // cert_conf.close();

    // #5 CSR 생성 && return request
    // log(info) << "[...]Generate CSR";
    rsp = generate_CSR_return_result(conf, key, csr, certificate_odata_id);
    // resource_save_json(this);// ??

    // 스키마상에는 CSR 생성까지만인데 CERT를 만드는 액션이 따로 없어서
    // 임의로 이부분에 구현해놓음
    generate_certificate(csr, key);
    
    // 리소스 검사하고

    return rsp;
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
 * @brief CSR 을 만들기위한 Config파일 생성함수
 * 
 * @param _config_filename config file name
 * @param _key_filename key file name 
 * @param _info config info
 */
void generate_ssl_config_file(fs::path _config_filename, fs::path _key_filename, CertContent _info)
{
    char cert_text[14][200];
    string key_length = to_string(CERTIFICATE_KEY_LENGTH);

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
    sprintf(cert_text[12], "organizationalUnitName\t=\"%s\"\n", _info.organizationUnit.c_str());	
    sprintf(cert_text[13], "commonName\t=\"%s\"\n", _info.commonName.c_str());
    
    ofstream cert_conf(_config_filename);
    for (int i = 0; i < 14; i++)
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
            
            odata_id["@odata.id"] = json::value::string(_target_id);
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

void generate_certificate(fs::path _csr, fs::path _key)
{
    log(info) << "[...]Generate Certificate";
    string cmd = "openssl req -x509 -in " + _csr.string() + " -key " + _key.string()
    + " -out " + TMP_CRT_FILE + " -days " + to_string(CERTIFICATE_VALID_DAYS);

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