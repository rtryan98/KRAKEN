#include <Yggdrasil/Yggdrasil.h>
#include <Yggdrasil/Common/Window/Input.h>
#include <cstdint>

class Game : public Ygg::Game
{
    virtual void Init() override
    {

    }

    virtual void Update() override
    {

    }

    virtual void Shutdown() override
    {

    }
} game;

int32_t main()
{
    Ygg::InitInfo initInfo{};
    initInfo.windowCreateInfo.title = "Yggdrasil Sandbox";
    initInfo.pGame = &game;

    Ygg::Init(&initInfo);
    Ygg::Run();
    return 0;
}
