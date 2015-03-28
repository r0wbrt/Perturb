
#include <DomainManager.h>

namespace Perturb
{

int DomainManager::genToken()
{
	static std::atomic<bool> lock(false);
	static int id = 5;
	int ret;
	
	//Spin Lock
	while(std::atomic_exchange_explicit(&lock, true, std::memory_order_acquire))
		  ;
	ret = ++id;
	std::atomic_store_explicit(&lock, false, std::memory_order_release);
	
	return ret;
}

DomainManager::DomainManager(Perturb::App& App) : App(App), Token(DomainManager::genToken()), Framework(App.getFramework())
{
	this->Receiver.RegisterHandler(&this->SyncReplyBuffer, &Theron::Catcher<Perturb::ActorSyncReplyMessage>::Push);
}
DomainManager::DomainManager(Perturb::App& App, int Token) : App(App), Token(Token), Framework(App.getFramework())
{
	this->Receiver.RegisterHandler(&this->SyncReplyBuffer, &Theron::Catcher<Perturb::ActorSyncReplyMessage>::Push);
}

int DomainManager::getToken()
{
	return this->Token;
}
int DomainManager::setToken(int Token)
{
	this->Token = Token;
	std::for_each(this->List.begin(), this->List.end(), 
	[&] (const Perturb::Address& Address) -> void
	{
		this->IssueNewTokenMessage(Address, Token);
	});
	return this->Token;
}


bool DomainManager::AddToDomain(Perturb::Address ActorAddress)
{
	this->List.push_back(ActorAddress);
	this->IssueNewTokenMessage(ActorAddress, this->Token);
}
	
bool DomainManager::RemoveFromDomain(Perturb::Address ActorAddress)
{
	std::vector<int> removeList;
	auto it = std::find(this->List.begin(), this->List.end(), ActorAddress);

	if(*it != ActorAddress)
		return false;

	this->List.erase(it);
	this->IssueNewTokenMessage(ActorAddress, 0);
	
	std::for_each(this->OutstandingSyncMessages.begin(), this->OutstandingSyncMessages.end(), 
	[&](const std::pair<int, Perturb::Address>& val) -> void 
	{
		if(val.second == ActorAddress)
			removeList.push_back(val.first);
	});

	std::for_each(removeList.begin(), removeList.end(), 
	[&](const int& id) -> void
	{
		this->OutstandingSyncMessages.erase(id);
	});
	
	return true;
}    
bool DomainManager::waitForSync()
{
	if(this->OutstandingSyncMessages.empty() == true)
		return true;

	while(this->OutstandingSyncMessages.empty() != true)
	{
		this->Receiver.Wait();
		if(this->SyncReplyBuffer.Empty()!=true)
		{
			Perturb::Address Address;
			Perturb::ActorSyncReplyMessage msg;
			this->SyncReplyBuffer.Pop(msg, Address);
			this->OutstandingSyncMessages.erase(msg.ID);
		}
	}

	return true;
}
bool DomainManager::isInSync()
{
	if(this->OutstandingSyncMessages.empty() == true)
		return true;
	
	return false;
}
bool DomainManager::ResetDomain()
{
	std::for_each(this->List.begin(), this->List.end(), 
	[&] (const Perturb::Address& Address) -> void
	{
		this->IssueResetMessage(Address);
	});
	return true;
}


void DomainManager::IssueNewTokenMessage(Perturb::Address Address, int Token)
{
	Perturb::ActorSyncRequestMessage msg;
	int SyncID = this->genNewSyncID();
	msg.ResetState = false;
	msg.ChangeToken = true;
	msg.Token = Token;
	msg.ID = SyncID;
	if(this->Framework.Send(msg, this->Receiver.GetAddress(), Address)!=true)
		return ;

	this->OutstandingSyncMessages[SyncID] = Address;
}
void DomainManager::IssueResetMessage(Perturb::Address Address)
{
	Perturb::ActorSyncRequestMessage msg;
	int SyncID = this->genNewSyncID();
	msg.ResetState = true;
	msg.ChangeToken = false;
	msg.Token = this->Token;
	msg.ID = SyncID;
	if(this->Framework.Send(msg, this->Receiver.GetAddress(),  Address)!=true)
		return ;

	this->OutstandingSyncMessages[SyncID] = Address;
}

int DomainManager::genNewSyncID()
{
	this->syncCounter++;
	return this->syncCounter;
}

};
