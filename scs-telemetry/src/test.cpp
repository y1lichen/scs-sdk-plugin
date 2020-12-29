//
// Created by jack on 12/28/20.
//
#define SCS_PLUGIN_MMF_NAME "/dev/shm/SCS/SCSTelemetry"
const char *scs_mmf_name = SCS_PLUGIN_MMF_NAME;

#include <sharedmemory.hpp>
#include <iostream>

int main(){
    auto telem_mem = new SharedMemory(scs_mmf_name, SCS_PLUGIN_MMF_SIZE);
    auto telem_ptr = static_cast<scsTelemetryMap_t *>(telem_mem->GetBuffer());
    telem_ptr->truck_f.speed = 12;
    std::cout << telem_ptr << std::endl;
}