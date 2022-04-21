#include "Bus.h"
#include "CPU.h"
#include "NES.h"
#include "Cartridge.h"
#include "WindowManager.h"

int main() 
{
    auto nes = NES();
    auto cartridge = Cartridge("/home/rysev/Git/nes-emu/tests/nestest.nes");

    if (!cartridge.is_valid()) {
        dbgln("Invalid cartridge");
        return 1;
    }

    nes.insert_cartridge(&cartridge);
    nes.reset();

    auto wm = WindowManager(&nes);
    wm.init();
    wm.run();
    
    return 0;
}
