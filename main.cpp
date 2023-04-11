#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>
#include <dlfcn.h>

#include <GTASA_STRUCTS.h>

MYMODCFG(net.kagikn.rusjj.glowingpickups, GTA:SA Glowing Pickups, 1.0, kagikn & RusJJ)
NEEDGAME(com.rockstargames.gtasa)


// Patched


// Savings
uintptr_t pGTASA;
void* hGTASA;

// Game Vars
CCamera* TheCamera;
unsigned int* m_snTimeInMilliseconds;
RwTexture** gpShadowExplosionTex;
CPickup* aPickups;

// Game Funcs
void (*RegisterCorona)(unsigned int, CEntity*, unsigned char, unsigned char, unsigned char, unsigned char, CVector const&, float, float, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, float, bool, float, bool, float, bool, bool);
void (*StoreStaticShadow)(unsigned int, unsigned char, RwTexture*, CVector*, float, float, float, float, short, unsigned char, unsigned char, unsigned char, float, float, float, bool, float);

// Own Funcs
void DoPickupGlowing(CPickup* pu)
{
    if(pu->m_nFlags.bDisabled || !pu->m_nFlags.bVisible || !pu->m_pObject) return;
    
    float distance = DistanceBetweenPoints(TheCamera->m_vecGameCamPos, pu->m_pObject->GetPosition());
    switch(pu->m_pObject->m_nModelIndex)
    {
        case 953:
        case 954:
        case 2782:
        {
            if(distance < 14.0f)
            {
                const short seed = (static_cast<unsigned short>(*m_snTimeInMilliseconds) + static_cast<unsigned short>((int)pu->m_pObject)) & 0x7FF;
                double sine = sin((double)(seed * 0.00306640635));
                if (static_cast<unsigned short>(sine) & 0x400) sine = 3.141592653589793116 * distance / 180.0;
                
                uint8_t intensity = (uint8_t)((14.0f - distance) * (0.5f * sine + 0.5f) * 0.0714285746f * 255.0f);
                
                RegisterCorona((unsigned int)(pu->m_pObject), NULL, intensity, intensity, intensity, 255, pu->m_pObject->GetPosition(),
                               0.6f, 40.0f, PICKUP_MINE_INACTIVE, FLARETYPE_NONE, false, false, 0, 0.0f, false, 1.5f, 0, 15.0f, false, true);
                StoreStaticShadow((unsigned int)(pu->m_pObject), 2, *gpShadowExplosionTex, &pu->m_pObject->GetPosition(), 2.0f, 0, 0, -2.0f, 0x10,
                                  intensity, intensity, intensity, 4.0f, 1.0f, 40.0f, false, 0.0f);
            }
            break;
        }
        
        case 1212:
        {
            if(distance < 20.0f)
            {
                const short seed = (static_cast<unsigned short>(*m_snTimeInMilliseconds) + static_cast<unsigned short>((int)pu->m_pObject)) & 0x7FF;
                double sine = sin((double)(seed * 0.00306640635));
                if (static_cast<unsigned short>(sine) & 0x400) sine = 3.141592653589793116 * distance / 180.0;
                
                uint8_t intensity = (uint8_t)((20.0f - distance) * (0.2f * sine + 0.3) * 0.05f * 64.0f);
                
                logger->Info("Upd %d %f %d", (int)pu->m_pObject->m_nModelIndex, distance, (int)intensity);
                RegisterCorona((unsigned int)(pu->m_pObject), NULL, 0, intensity, 0, 255, pu->m_pObject->GetPosition(),
                               0.25f, 40.0f, PICKUP_MINE_INACTIVE, FLARETYPE_NONE, false, false, 0, 0.0f, false, 1.5f, 0, 15.0f, false, true);
                StoreStaticShadow((unsigned int)(pu->m_pObject), 2, *gpShadowExplosionTex, &pu->m_pObject->GetPosition(), 2.0f, 0, 0, -2.0f, 0x10,
                                  0, intensity, 0, 4.0f, 1.0f, 40.0f, false, 0.0f);
            }
            break;
        }
        
        default:
        {
            if(pu->m_nPickupType == PICKUP_NONE) break;
            
            bool glowInner, glowOuter;
            
            uint8_t glowStage1 = (uint8_t)((*m_snTimeInMilliseconds >> 9) & 7);
            uint8_t glowStage2 = (glowStage1 - 1) & 7;
            
            if(glowStage1 == 3) glowOuter = (rand() & 3) != 0;
            else if(glowStage1 > 3) glowOuter = true;
            
            if(glowStage2 == 3) glowInner = (rand() & 3) != 0;
            else if(glowStage2 > 3) glowInner = true;
            
            break;
        }
    }
}

// Other
DECL_HOOKv(ProcessGame)
{
    ProcessGame();
    for(int i = 0; i < 620; ++i)
    {
        DoPickupGlowing(&aPickups[i]);
    }
}

extern "C" void OnModLoad()
{
    logger->SetTag("GlowingPickupsSAA");
    pGTASA = aml->GetLib("libGTASA.so");
    hGTASA = aml->GetLibHandle("libGTASA.so");
    
    SET_TO(aPickups, *(uintptr_t*)(pGTASA + 0x678BF8));
    SET_TO(TheCamera, aml->GetSym(hGTASA, "TheCamera"));
    SET_TO(m_snTimeInMilliseconds, aml->GetSym(hGTASA, "_ZN6CTimer22m_snTimeInMillisecondsE"));
    SET_TO(gpShadowExplosionTex, aml->GetSym(hGTASA, "gpShadowExplosionTex"));
    
    SET_TO(RegisterCorona, aml->GetSym(hGTASA, "_ZN8CCoronas14RegisterCoronaEjP7CEntityhhhhRK7CVectorffhhhhhfbfbfbb"));
    SET_TO(StoreStaticShadow, aml->GetSym(hGTASA, "_ZN8CShadows17StoreStaticShadowEjhP9RwTextureP7CVectorffffshhhfffbf"));
    
    HOOK(ProcessGame, aml->GetSym(hGTASA, "_ZN5CGame7ProcessEv"));
}
