#pragma once
#include <string>
#include <vector>
#include <map>

class Console
{
public:
    class Command
    {
    public:
        Command(const std::string& sign) : m_Signature(sign) {}

        virtual void Execute(void* context) = 0;

        bool Is(const std::string& sig)
        {
            return m_Signature == sig;
        }
    protected:
        std::string m_Signature;
    };

    class CommandProducer
    {
    public:
        CommandProducer(const std::string& sign) : m_Signature(sign) {}
        //Command(const std::string& signature) : m_Signature(signature) {}

        virtual bool Is(const std::string& signature) { return m_Signature == signature;  }

        virtual Console::Command* Produce(const std::string& args) = 0;
    protected:
        std::string m_Signature;
    };

    Console(const std::vector < CommandProducer* > producers) : m_Open(true), m_CommandProducers(producers) {}

    bool Produce(const std::string& signature, const std::string& argsStr)
    {
        for (CommandProducer*& c : m_CommandProducers)
        {
            if (c->Is(signature))
            {
                Command* pc = c->Produce(argsStr);
                if (pc)
                    m_Commands.push_back(pc);
                return !!pc;
            }
        }

        return false;
    }

    void FetchCommands(const std::string& sig, std::vector<Command*>& commands)
    {
        //mb mark for deletion here instead of full clear
        for (Command* command : m_Commands)
        {
            if (command->Is(sig))
                commands.push_back(command);
        }
    }

    void Show() { m_Open = true; }
    void Process();
private:
    bool m_Open;
    char m_InputBuffer[256];
    std::vector<std::string> m_Log;
    //std::map<std::pair<std::string, int>, Command*
    std::vector<Command*> m_Commands;
    std::vector<CommandProducer*> m_CommandProducers;
};