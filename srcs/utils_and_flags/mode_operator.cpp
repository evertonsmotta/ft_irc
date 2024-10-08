#include "../../includes/Server.hpp"

/*
	* MODE_OPERATOR
	* 	mode #channel -/+o
	* 	mode #channel -/+o member_target
*/
void Server::mode_operator(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.size() < 2 || params.size() > 3)
	{
		sendMessage(clientFd, "461 " + client.getNickName() + " MODE :Not enough parameters\n");
		return;
	}

	std::map<std::string, Channel>::iterator it = _channels.find(params[0]);
	if (it == _channels.end())
	{
		sendMessage(clientFd, "403 " + client.getNickName() + " " + params[0] + " :No such channel\n");
		return;
	}

	if (!it->second.isMember(client))
	{
		sendMessage(clientFd, "442 " + client.getNickName() + " " + params[0] + " :You're not on that channel\n");
		return;
	}

	if (!it->second.isOperator(client))
	{
		sendMessage(clientFd, "482 " + client.getNickName() + " " + params[0] + " :You're not channel operator\n");
		return;
	}

	if (params.size() == 2)
	{
		if (params[1] == "+o")
		{
			it->second.setOperatorMode(true);
			// sendMessage(clientFd, "Only operators can change channel modes now.\n");
		}
		else if (params[1] == "-o")
		{
			it->second.setOperatorMode(false);
			// sendMessage(clientFd, "Any user can change channel modes now.\n");
		}
		else
		{
			sendMessage(clientFd, "472 " + client.getNickName() + " " + params[1] + " :is an unknown mode character\n");
		}
		return;
	}

	if (params.size() == 3)
	{
		if (!it->second.isMember(params[2]))
		{
			sendMessage(clientFd, "401 " + client.getNickName() + " " + params[2] + " :No such nick/channel\n");
			return;
		}

		if (params[1] == "+o")
		{
			if (it->second.isOperator(params[2]))
			{
				sendMessage(clientFd, "433 " + client.getNickName() + " " + params[2] + " :Nickname is already operator\n");
				return;
			}
			it->second.setOperator(params[2]);
			// std::cout << "Added operator: " << params[2] << std::endl;
			// sendMessage(clientFd, params[2] + " is now an operator of " + params[0] + ".\n");
		}
		else if (params[1] == "-o")
		{
			if (it->second.isOperator(params[2]))
			{
				if (it->second.isCreator(params[2]))
				{
					sendMessage(clientFd, "The channel creator cannot be removed of operators list\n");
					return;
				}
				it->second.removeOperator(params[2]);
				// sendMessage(clientFd, params[2] + " is no longer an operator of " + params[0] + "!\n");
				return;
			}
		}
		else
		{
			sendMessage(clientFd, "472 " + client.getNickName() + " " + params[1] + " :is an unknown mode character\n");

		}
		return;
	}
}
