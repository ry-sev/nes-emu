#include "Bus.h"
#include "CPU.h"
#include "NES.h"
#include "Cartridge.h"
#include "WindowManager.h"
#include <memory>

int main() 
{
    auto nes = std::make_shared<NES>();

    auto cartridge = std::make_shared<Cartridge>("../tests/ntsc_torture.nes");

    if (!cartridge->is_valid()) {
        dbgln("Invalid cartridge");
        return 1;
    }

    nes->insert_cartridge(cartridge);
    nes->reset();

    auto wm = WindowManager(nes);
    wm.init();
    wm.run();

    return 0;
}
