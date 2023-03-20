#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <fstream>   // ifstream, ofstream
#include <sstream>   // istringstream
#include <iomanip>
#include <chrono>
#include <numeric>
#include <bitset>

using namespace std;

//���������؂蕶��(delimiter)�ŕ�������֐�
vector<string> split(string str, char delimiter) {
    vector<string> result;
    size_t pos = 0;
    string token;
    while ((pos = str.find(delimiter)) != string::npos) {
        token = str.substr(0, pos);
        result.push_back(token);
        str.erase(0, pos + 1);
    }
    result.push_back(str);
    return result;
}

typedef long long TimeStamp;
//IP�A�h���X��\��
struct Address {
    string address;
    Address():address() {}
    Address(string& add) :address(add) {}
    //�A�h���X�̃T�u�l�b�g���擾
    string GetSubnet()const {

        string res = "";
        // IP�A�h���X�ƃT�u�l�b�g�𕪊�
        auto add = split(address, '/');
        int subnet_len = stoi(add[1]);

        // IP�A�h���X���o�C�g�P�ʂɕ���
        auto byte_str = split(add[0], '.');
        int bytes[4];
        for (int i = 0; i < 4; i++) {
            bytes[i] = stoi(byte_str[i]);
        }

        bitset<8> address0 = bytes[0];
        bitset<8> subnet0;
        for (int i = 0; i < 8; i++) {
            if (i < subnet_len) {
                subnet0.set(i);
            }
        }
        res += to_string((address0 & subnet0).to_ulong());
        res += ".";
        subnet_len -= 8;
        if (subnet_len <= 0) {
            return res + "0.0.0";
        }

        bitset<8> address1 = bytes[1];
        bitset<8> subnet1;
        for (int i = 0; i < 8; i++) {
            if (i < subnet_len) {
                subnet1.set(i);
            }
        }
        res += to_string((address1 & subnet1).to_ulong());
        res += ".";
        subnet_len -= 8;
        if (subnet_len <= 0) {
            return res + "0.0";
        }

        bitset<8> address2 = bytes[2];
        bitset<8> subnet2;
        for (int i = 0; i < 8; i++) {
            if (i < subnet_len) {
                subnet2.set(i);
            }
        }
        res += to_string((address2 & subnet2).to_ulong());
        res += ".";
        subnet_len -= 8;
        if (subnet_len <= 0) {
            return res + "0";
        }

        bitset<8> address3 = bytes[3];
        bitset<8> subnet3;
        for (int i = 0; i < 8; i++) {
            if (i < subnet_len) {
                subnet3.set(i);
            }
        }
        res += to_string((address3 & subnet3).to_ulong());
        return res;
    }
};

bool operator<(const Address& lgt,const Address& rgt) {//map�Ŏg�����߂̒�`
    return lgt.address < rgt.address;
}

struct Log {
    TimeStamp timestamp;
    Address address;
    string response;
};

//���t�̕������timestamp�ɕϊ�
TimeStamp DateToTimestamp(const string& date) {
    tm time = tm();
    istringstream time_stream(date);
    time_stream >> get_time(&time, "%Y%m%d%H%M%S");
    auto tp = chrono::system_clock::from_time_t(mktime(&time));
    chrono::milliseconds ms(chrono::duration_cast<chrono::milliseconds>(tp.time_since_epoch()));
    //cout << "convert 1:" << date<<" to "<<ms.count() << endl;
    return ms.count();
}
//timestamp����t�̕�����ɕϊ�
string TimestampToDate(TimeStamp timestamp) {
    if (timestamp == -1)return "-1";
    time_t timer;
    struct tm* tm;
    char datetime[30];
    timer = (time_t)(timestamp/1000);
    tm = localtime(&timer);
    strftime(datetime, 30, "%Y%m%d%H%M%S", tm);
    //cout << "convert 2:" << timestamp << " to " << datetime << endl;
    return datetime;
}

vector<Log> GetLog(const vector<string>& filedata) {
    vector<Log> res;
    for (auto& i : filedata) {
        auto dat = split(i, ',');
        string time_s = dat[0];
        string address = dat[1];
        string response = dat[2];
        // std::cout <<"str: "<< time_s<<" Timestamp: " << ms.count() << std::endl;
        res.push_back({
             DateToTimestamp(time_s),
             address,
             response
            });
    }
    return res;
}

// =====================================================================================================================
// �ݖ�P
// =====================================================================================================================
// �̏��ԂɂȂ��Ă���A�h���X�ƁA���̃A�h���X�̌̏���Ԃ��o��
vector<pair<Address, pair<TimeStamp, TimeStamp>>> GetBrokenServers1(const vector<Log>& logs) {
    vector<pair<Address, pair<TimeStamp, TimeStamp>>> res;//�o��
    map<Address, TimeStamp> s;//�̏��ԂɂȂ��Ă���T�[�o�[�̃A�h���X�istring�j�ƁA���̊J�n���Ԃ�timestamp�iint�j
    for (const auto& log : logs) {
        if (log.response == "-") {
            if (s.find(log.address) == s.end()) {
                s[log.address] =  log.timestamp;
            }
        }
        else if (s.find(log.address) != s.end()) {//����܂Ō̏Ⴕ�Ă��āA���A�����ꍇ
            res.push_back({ log.address, {s[log.address], log.timestamp } });//�̏���Ԃ̋L�^
            s.erase(log.address);//�̏Ⴕ�Ă��邩�̃t���O������
        }
    }
    for (const auto& i : s) {//�̏��Ԃ���񕜂��Ă��Ȃ��T�[�o�[���������ꍇ�A�ŏ��Ɍ̏Ⴕ����������-1�܂ł��̏���ԂƂ���
        res.push_back({ i.first, {i.second, -1} });
    }
    return res;
}

// =====================================================================================================================
// �ݖ�Q
// =====================================================================================================================
//N��ȏ�^�C���A�E�g�����ꍇ�݂̂��̏��ԂƂ��Ĉ���
vector<pair<Address, pair<TimeStamp, TimeStamp>>> GetBrokenServers2(const vector<Log>& logs, int threshold_N = 0) {
    vector<pair<Address, pair<TimeStamp, TimeStamp>>> res;//�o��
    map<Address, pair<TimeStamp, int>> s;//�̏��ԂɂȂ��Ă���T�[�o�[�̃A�h���X�istring�j�ƁA���̊J�n���Ԃ�timestamp�iint�j�A�����ĉ���A�����Č̏��ԂɂȂ������̃J�E���g�iint�j
    for (const auto& log : logs) {
        if (log.response == "-") {
            if (s.find(log.address) == s.end()) {
                s[log.address] = { log.timestamp, 1 };
            }
            else {
                s[log.address].second++;
            }
        }
        else if (s.find(log.address) != s.end()) {//����܂Ō̏Ⴕ�Ă��āA���A�����ꍇ
            if (s[log.address].second >= threshold_N) {//�̏���Ԃ�臒l�𒴂��Ă���΋L�^
                res.push_back({ log.address, {s[log.address].first, log.timestamp} });
            }
            s.erase(log.address);//�̏Ⴕ�Ă��邩�̃t���O������
        }
    }
    for (const auto& i : s) {//�̏��Ԃ���񕜂��Ă��Ȃ��T�[�o�[���������ꍇ�A�ŏ��Ɍ̏Ⴕ����������-1�܂ł��̏���ԂƂ���
        res.push_back({ i.first, {i.second.first, -1} });
    }
    return res;
}

// =====================================================================================================================
//�ݖ�R
// =====================================================================================================================
//����m��̕��ω������Ԃ�t�~���b�𒴂����ꍇ���ߕ��׏�ԂƂ݂Ȃ��A�ߕ��׏�ԂƂȂ��Ă�����Ԃ��o��
struct BrokenServer3Res {
    vector<pair<Address, pair<TimeStamp, TimeStamp>>> broken_time;//�e�T�[�o�[�̌̏����
    vector<pair<Address, pair<TimeStamp, TimeStamp>>> overload_time;//�e�T�[�o�[�̉ߕ��׊���
};
BrokenServer3Res GetBrokenServers3(const vector<Log>& logs, int m, int t, int threshold_N = 0) {
    vector<pair<Address, pair<TimeStamp, TimeStamp>>> res;//�e�T�[�o�[�̌̏����
    map<Address, pair<TimeStamp, int>> s;//�̏��ԂɂȂ��Ă���T�[�o�[�̃A�h���X�istring�j�ƁA���̊J�n���Ԃ�timestamp�iint�j�A�����ĉ���A�����Č̏��ԂɂȂ������̃J�E���g�iint�j
    map<Address, vector<TimeStamp>> t_history;//���̃T�[�o�[�istring�j�̉������Ԃ̗���
    map<Address, TimeStamp> overload;//���̃T�[�o�[�istring�j���ŏ��ɉߕ��׏�ԂɂȂ����Ƃ���timestamp�iint�j
    vector<pair<Address, pair<TimeStamp, TimeStamp>>> overload_res;//���̃T�[�o�[�istring�j�̉ߕ��׏�Ԃ̊���

    for (const auto& log : logs) {
        if (log.response == "-") {
            if (s.find(log.address) == s.end()) {
                s[log.address] = { log.timestamp ,1 };
            }
            else {
                s[log.address].second++;
            }
            //����m��̒��ɉ����Ȃ�������ꍇ�A���̃T�[�o�[�͉ߕ��׏�Ԃł�����ƍl���A�����Ȃ���ping��臒l�ł���t�~���b�Ƃ���B
            t_history[log.address].push_back(t);
        }
        else {
            if (s.find(log.address) != s.end()) {//����܂Ō̏Ⴕ�Ă��āA���A�����ꍇ
                if (s[log.address].second >= threshold_N) {//�̏���Ԃ�臒l�𒴂��Ă���΋L�^
                    res.push_back({ log.address, {s[log.address].first, log.timestamp} });
                }
                s.erase(log.address);//�̏Ⴕ�Ă��邩�̃t���O������
            }
            t_history[log.address].push_back(atoi(log.response.c_str()));
        }
        if (t_history[log.address].size() >= m) {//�T�[�o�[��ping�������\���ʗ��܂�����
            auto ave = accumulate(t_history[log.address].end() - m, t_history[log.address].end(), 0) / float(m);
            //  cout << ave << endl;
            if (ave > float(t)) {//�ߕ��׏�Ԃ̏ꍇ
                if (overload.find(log.address) == overload.end()) {
                    // cout << "!!!!" << endl;
                    overload[log.address] = log.timestamp;
                }
            }
            else {
                if (overload.find(log.address) != overload.end()) {//����܂ŉߕ��׏�ԂŁA�Ȃ����ߕ��׏�Ԃ��������ꂽ�ꍇ
                    overload_res.push_back({ log.address, {overload[log.address], log.timestamp} });
                    overload.erase(log.address);
                }
            }
        }
    }
    for (const auto& i : s) {//�̏��Ԃ���񕜂��Ă��Ȃ��T�[�o�[���������ꍇ�A�ŏ��Ɍ̏Ⴕ����������-1�܂ł��̏���ԂƂ���
        res.push_back({ i.first, {i.second.first, -1} });
    }
    for (const auto& i : overload) {//�ߕ��׏�Ԃ���񕜂��Ă��Ȃ��T�[�o�[���������ꍇ�A�ŏ��ɉߕ��ׂɂȂ�����������-1�܂ł��ߕ��׊��ԂƂ���
        overload_res.push_back({ i.first, {i.second, -1} });
    }
    return { res, overload_res };
}

// =====================================================================================================================
// �ݖ�S�i�������j
// =====================================================================================================================
// �e�T�u�l�b�g���̌̏���Ԃ��o��
// �T�u�l�b�g�����ׂẴT�[�o�[��N�񉞓��Ȃ��Ȃ�T�u�l�b�g���̏�
struct BrokenServer4Res {
    vector<pair<Address, TimeStamp>> broken_time;
    map<Address, TimeStamp> overload_time;
    vector<pair<string, TimeStamp>> mask_broken_time;//�T�u�l�b�g���̌̏����
};
BrokenServer4Res GetBrokenServers4(const vector<Log>& log, int m, int t, int threshold_N = 0) {
    vector<pair<Address, TimeStamp>> res;//�o��
    map<Address, pair<TimeStamp, int>> s;//�̏��ԂɂȂ��Ă���T�[�o�[�̃A�h���X�istring�j�ƁA���̊J�n���Ԃ�timestamp�iint�j�A�����ĉ���A�����Č̏��ԂɂȂ������̃J�E���g�iint�j
    map<Address, vector<TimeStamp>> t_history;//���̃T�[�o�[�istring�j�̉������Ԃ̗���
    map<Address, TimeStamp> overload;//���̃T�[�o�[�istring�j���ŏ��ɉߕ��׏�ԂɂȂ����Ƃ���timestamp�iint�j
    map<Address, TimeStamp> overload_res;//���̃T�[�o�[�istring�j�̉ߕ��׏�Ԃ̊��ԁiint�j
    vector<pair<string, TimeStamp>> mask_broken_time;//�T�u�l�b�g���̌̏����

    for (const auto& i : log) {
        if (t_history[i.address].size() >= m) {//�T�[�o�[��ping�������\���ʗ��܂�����
            auto ave = accumulate(t_history[i.address].end() - m, t_history[i.address].end(), 0) / float(m);
            if (ave > float(t)) {//�ߕ��׏�Ԃ̏ꍇ
                overload[i.address] = i.timestamp;
            }
            else {
                if (overload.find(i.address) != overload.end()) {
                    overload_res[i.address] += i.timestamp - overload[i.address];
                }
            }
        }
        if (i.response == "-") {
            if (s.find(i.address) == s.end()) {
                s[i.address] = { i.timestamp ,1 };
            }
            else {
                s[i.address].second++;
            }
        }
        else if (s.find(i.address) != s.end()) {
            t_history[i.address].push_back(atoi(i.response.c_str()));
            if (s[i.address].second >= threshold_N) {//�̏���Ԃ�臒l�𒴂��Ă���΋L�^
                res.push_back({ i.address,i.timestamp - s[i.address].first });
                mask_broken_time.push_back({ i.address.GetSubnet(),i.timestamp - s[i.address].first });
            }
            s.erase(i.address);
        }
    }
    for (auto& i : s) {
        res.push_back({ i.first, -1 });
    }
    return {
        res,
        overload_res
    };
}

// �t�@�C������e�X�g�p���͂�ǂݍ��݁A���s����֐�
void CodeTest(const string& filename) {
    ifstream reading_file(filename, ios::in);
    if (!reading_file) {
        cerr << "file not found" << endl;
        return;
    }
    string reading_line_buffer;
    vector<string> fileData;
    getline(reading_file, reading_line_buffer);
   // cout << reading_line_buffer<<endl;
    int q = atoi(reading_line_buffer.c_str());
    int n;
    int m, t;
    if (q == 2 || q == 3 || q == 4) {
        getline(reading_file, reading_line_buffer);
        n = atoi(reading_line_buffer.c_str());
    }
    if (q == 3 || q == 4) {
        getline(reading_file, reading_line_buffer, ' ');
        m = atoi(reading_line_buffer.c_str());
        getline(reading_file, reading_line_buffer);
        t = atoi(reading_line_buffer.c_str());
    }
    while (!reading_file.eof())
    {
        getline(reading_file, reading_line_buffer);
        fileData.push_back(reading_line_buffer);
    }
    auto log = GetLog(fileData);
    /*for (auto& i : log) {
        cout << i.address.address << " " << i.response << " subnet: " << i.address.GetSubnet() << endl;
    }*/
    if (q == 1) {
        auto data = GetBrokenServers1(log);
        for (auto& i : data) {
            cout << i.first.address << " " << TimestampToDate(i.second.first) << " " << TimestampToDate(i.second.second) << endl;
        }
    }
    else if (q == 2) {
        auto data = GetBrokenServers2(log, n);
        for (auto& i : data) {
            cout << i.first.address << " " << TimestampToDate(i.second.first) << " " << TimestampToDate(i.second.second) << endl;
        }
    }
    else if (q == 3) {
        auto data = GetBrokenServers3(log, m, t, n);
        for (auto& i : data.broken_time) {
            cout << i.first.address << " " << TimestampToDate(i.second.first) << " " << TimestampToDate(i.second.second) << endl;
        }
        cout << "===" << endl;
        for (auto& i : data.overload_time) {
            cout << i.first.address << " " << TimestampToDate(i.second.first) << " " << TimestampToDate(i.second.second) << endl;
        }
    }
    else if (q == 4) {
        /*auto data = GetBrokenServers4(log, m, t, n);
        for (auto& i : data.broken_time) {
            cout << i.first.address << " " << TimestampToDate(i.second.first) << " " << TimestampToDate(i.second.second) << endl;
        }
        cout << "===" << endl;
        for (auto& i : data.overload_time) {
            cout << i.first.address << " " << TimestampToDate(i.second.first) << " " << TimestampToDate(i.second.second) << endl;
        }
        for (auto& i : data.mask_broken_time) {
            cout << i.first.address << " " << TimestampToDate(i.second.first) << " " << TimestampToDate(i.second.second) << endl;
        }*/
    }
}

int main() {
    ifstream reading_file("files.txt", ios::in);//�e�X�g�p�R�[�h�̈ꗗ��ǂݍ���
    string reading_line_buffer;
    vector<string> files;
    while (!reading_file.eof())
    {
        getline(reading_file, reading_line_buffer);
        files.push_back(reading_line_buffer);
    }
    for (const auto& file : files) {
        cout <<"filename: "<< file << endl;
        CodeTest(file);
    }
}