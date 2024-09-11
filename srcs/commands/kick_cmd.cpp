#include "../../includes/Server.hpp"


void Server::sendToChannel(const std::string& channelName, const std::string& message)
{
    // Verifica se o canal existe
    std::map<std::string, Channel>::iterator it = _channels.find(channelName);
    if (it != _channels.end())
    {
        Channel& channel = it->second;
        std::map<int, Client*> members = channel.getMembers();

        // Envia a mensagem para todos os membros do canal
        for (std::map<int, Client*>::iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt)
        {
            Client* client = memberIt->second;
            if (client)
            {
                int clientFd = client->getClientFd();
                sendMessage(clientFd, message);
            }
        }
    }
    else
    {
        std::cerr << "Channel " << channelName << " does not exist." << std::endl;
        // Se necessário, envie uma mensagem de erro para o cliente que enviou o comando
    }
}


std::vector<std::string> filterPortStrings(std::vector<std::string> params) {
    std::vector<std::string> filteredParams;

    for (std::vector<std::string>::const_iterator it = params.begin(); it != params.end(); ++it) {
        const std::string& param = *it;
        if (param.find("port_") != 0) { // Checks if the string does not start with "port_"
            filteredParams.push_back(param);
        }
    }

    return filteredParams;
}



void Server::kick_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
    if (params.size() < 2)
    {
        sendMessage(clientFd, "461 KICK :Not enough parameters\r\n");
        return;
    }
	std::vector<std::string> newParams = filterPortStrings(params);

    std::string channelName = newParams[0];
    std::string nickName = newParams[1];
    // std::string comment = (newParams.size() > 2) ? newParams[2] : "";

    if (isChannelExist(channelName))
    {
        std::map<std::string, Channel>::iterator it = _channels.find(channelName);
        if (it != _channels.end())
        {
            Channel& channel = it->second;
            if (channel.isOperator(client))
            {
                if (channel.isMember(nickName))
                {
                    // Envia a mensagem KICK para todos os membros do canal
                    std::string kickMessage = ":" + client.getNickName() + " KICK " + channelName + " " + nickName;
                    // if (!comment.empty())
                    //     kickMessage += " :" + comment;
                    kickMessage += "\r\n";
                    sendToChannel(channelName, kickMessage);
                    // Notifica o cliente que executou o comando
                    sendMessage(clientFd, "You have kicked " + nickName + " from channel " + channelName + ".\r\n");

					// Remove o membro do canal
                    channel.removeMember(nickName);
                }
                else
                {
                    sendMessage(clientFd, "441 " + nickName + " " + channelName + " :They aren't on that channel\r\n");
                }
            }
            else
            {
                sendMessage(clientFd, "482 " + channelName + " :You're not channel operator\r\n");
            }
        }
    }
    else
    {
        sendMessage(clientFd, "403 " + channelName + " :No such channel\r\n");
		//std::cerr << "Received KICK command: Channel=" << channelName << " User=" << nickName << " Reason=" << comment << std::endl;

    }
}



// 3.2.8 Comando de chute

//       Comando: KICK
//    Parâmetros: <canal> *( "," <canal> ) <usuário> *( "," <usuário> )
//                [<comentário>]

//    O comando KICK pode ser usado para solicitar a remoção forçada de um usuário
//    de um canal. Isso faz com que o <usuário> SEPARE do <canal> por
//    força. Para que a mensagem seja sintaticamente correta, DEVE haver
//    um parâmetro de canal e vários parâmetros de usuário, ou tantos
//    parâmetros do canal, pois há parâmetros do usuário. Se um "comentário" for
//    dado, isso será enviado em vez da mensagem padrão, o apelido
//    do usuário que emite o KICK.

//    O servidor NÃO DEVE enviar mensagens KICK com múltiplos canais ou
//    usuários para clientes. Isso é necessariamente para manter o retrocesso
//    compatibilidade com software cliente antigo.

//  Examples:

//    KICK &Melbourne Matthew         ; Command to kick Matthew from
//                                    &Melbourne

//    KICK #Finnish John :Speaking English
//                                    ; Command to kick John from #Finnish
//                                    using "Speaking English" as the
//                                    reason (comment).

//    :WiZ!jto@tolsun.oulu.fi KICK #Finnish John
//                                    ; KICK message on channel #Finnish
                                //    from WiZ to remove John from channel
