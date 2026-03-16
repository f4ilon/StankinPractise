#include "Packets.hpp"

char* pack(Message data) {
    int len_type = data.type.size();
    int len_fromUser = data.fromUser.size();
    int len_message = data.message.size();
    char* pack_data = new char[len_fromUser+len_message+len_type+2];
    for (int i=0;i<len_type;i++) {
        pack_data[i] = data.type[i];
    }
    pack_data[len_type] = '~'; //Спец символ по которому раздиляются данные (если он появится в сообщении все сломается)
    for (int i=0;i<len_fromUser;i++) {
        pack_data[i+len_type+1] = data.fromUser[i];
    }
    pack_data[len_type+1+len_fromUser] = '~';
    for (int i=0;i<len_message;i++) {
        pack_data[i+len_type+len_fromUser+2] = data.message[i];
    }
    return pack_data;
}

Message unpack(char* pack_data) {
    Message data;
    std::string type = "";
    std::string fromUser = "";
    std::string message = "";
    int i = 0;
    while (pack_data[i] != '~') {
        type.push_back(pack_data[i]);
        i++;
    }
    i++;
    while (pack_data[i] != '~') {
        fromUser.push_back(pack_data[i]);
        i++;
    }
    i++;
    while (pack_data[i] != '~') {
        message.push_back(pack_data[i]);
        i++;
    }
    data.fromUser = fromUser;
    data.type = type;
    data.fromUser = message;
    return data;
}
