#include <KRAKEN/KRAKEN.h>

int32_t main()
{
    kraken::ApplicationCreateInfo createInfo{};
    kraken::Application app{ createInfo };
    app.run();
    return 0;
}
