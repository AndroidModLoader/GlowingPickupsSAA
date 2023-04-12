#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>
#include <unordered_map>

#include <GTASA_STRUCTS.h>

MYMODCFG(net.kagikn.rusjj.glowingpickups, GTA:SA Glowing Pickups, 1.0, kagikn & RusJJ)
NEEDGAME(com.rockstargames.gtasa)


// Patched


// Savings
uintptr_t pGTASA;
void* hGTASA;

extern std::unordered_map<uint16_t, CRGBA> PickupColorsMap;
namespace PickupColors
{
    const CRGBA Default(0x80, 0x80, 0x80);
    const CRGBA Melee(0x61, 0xC2, 0xF7);
    const CRGBA Throwable(0x1B, 0x59, 0x82);
    const CRGBA Handgun(0x95, 0xC2, 0x18);
    const CRGBA Shotgun(0x2D, 0x9B, 0x5A);
    const CRGBA Submachine(0xFF, 0xE3, 0x4F);
    const CRGBA Assault(0xFE, 0x89, 0x00);
    const CRGBA Sniper(0xF9, 0x83, 0xD7);
    const CRGBA Heavy(0xA4, 0x28, 0xB2);
    const CRGBA Misc(0x45, 0x45, 0x45);
    const CRGBA Health(0xFF, 0x64, 0x64);
    const CRGBA Armour(0x80, 0xFF, 0x80);
    
    const CRGBA Gift(0xF3, 0x44, 0x67);
    const CRGBA Special1(0xA6, 0xE2, 0xD2);
    const CRGBA Special2(0x3A, 0x3D, 0xD9);
    const CRGBA PropertySale(0x64, 0xFF, 0x64);
    const CRGBA PropertyLocked(0x64, 0x64, 0xFF);
    const CRGBA Yellow(0xFF, 0xFF, 0x64);
    const CRGBA Purple(0xFF, 0x8B, 0xF4);
    
    inline CRGBA FindFor(uint16_t modelIdx)
    {
        auto it = PickupColorsMap.find(modelIdx);
        if(it == PickupColorsMap.end()) return PickupColors::Default;
        return it->second;
    }
};
std::unordered_map<uint16_t, CRGBA> PickupColorsMap {
    {372, PickupColors::Submachine},{352, PickupColors::Submachine},{353, PickupColors::Submachine},
    {358, PickupColors::Sniper},{357, PickupColors::Sniper},{1239, PickupColors::Yellow},
    {},{},{},
    {},{},{},
    {},{},{},
    {},{},{},
    {},{},{},
    {},{},{},
};

// Game Vars
CCamera* TheCamera;
unsigned int* m_snTimeInMilliseconds;
RwTexture** gpShadowExplosionTex;
CPickup* aPickups;

// Game Funcs
void (*RegisterCorona)(unsigned int, CEntity*, unsigned char, unsigned char, unsigned char, unsigned char, CVector const&, float, float, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, float, bool, float, bool, float, bool, bool);
void (*StoreStaticShadow)(unsigned int, unsigned char, RwTexture*, CVector*, float, float, float, float, short, unsigned char, unsigned char, unsigned char, float, float, float, bool, float);
void (*AddLight)(unsigned char, CVector, CVector, float, float, float, float, unsigned char, bool, CEntity*);

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
            
            CRGBA clr = PickupColors::FindFor(pu->m_pObject->m_nModelIndex);
            
            if(glowOuter)
            {
                RegisterCorona((unsigned int)(pu->m_pObject), NULL, (uint8_t)(clr.r * 0.45f), (uint8_t)(clr.g * 0.45f), (uint8_t)(clr.b * 0.45f), 255, pu->m_pObject->GetPosition(),
                               0.76f, 65.0f, PICKUP_MINE_INACTIVE, FLARETYPE_NONE, false, false, 0, 0.0f, false, -0.4f, 0, 15.0f, false, true);
                StoreStaticShadow((unsigned int)(pu->m_pObject), 2, *gpShadowExplosionTex, &pu->m_pObject->GetPosition(), 2.0f, 0, 0, -2.0f, 0x10,
                                  (uint8_t)(clr.r * 0.3f), (uint8_t)(clr.g * 0.3f), (uint8_t)(clr.b * 0.3f), 4.0f, 1.0f, 40.0f, false, 0.0f);
                                  
                float lightRange = (rand() % 0xF) * 0.1f + 3.0f;
                AddLight(0, pu->m_pObject->GetPosition(), CVector(), lightRange, (uint8_t)(clr.r * 0.0039f), (uint8_t)(clr.g * 0.0039f), (uint8_t)(clr.b * 0.0039f), 0, true, NULL);
            }
            else
            {
                RegisterCorona((unsigned int)(pu->m_pObject), NULL, 0, 0, 0, 255, pu->m_pObject->GetPosition(),
                               0.76f, 65.0f, PICKUP_MINE_INACTIVE, FLARETYPE_NONE, false, false, 0, 0.0f, false, -0.4f, 0, 15.0f, false, true);
            }
            
            if(glowInner)
            {
                RegisterCorona((unsigned int)(pu->m_pObject)+1, NULL, (uint8_t)(clr.r * 0.45f), (uint8_t)(clr.g * 0.45f), (uint8_t)(clr.b * 0.45f), 255, pu->m_pObject->GetPosition(),
                               0.6f, 65.0f, PICKUP_MINE_INACTIVE, FLARETYPE_NONE, false, false, 0, 0.0f, false, -0.4f, 0, 15.0f, false, true);
                StoreStaticShadow((unsigned int)(pu->m_pObject), 2, *gpShadowExplosionTex, &pu->m_pObject->GetPosition(), 2.0f, 0, 0, -2.0f, 0x10,
                                  (uint8_t)(clr.r * 0.3f), (uint8_t)(clr.g * 0.3f), (uint8_t)(clr.b * 0.3f), 4.0f, 1.0f, 40.0f, false, 0.0f);
            }
            else
            {
                RegisterCorona((unsigned int)(pu->m_pObject)+1, NULL, 0, 0, 0, 255, pu->m_pObject->GetPosition(),
                               0.6f, 65.0f, PICKUP_MINE_INACTIVE, FLARETYPE_NONE, false, false, 0, 0.0f, false, -0.4f, 0, 15.0f, false, true);
            }
            
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
    SET_TO(AddLight, aml->GetSym(hGTASA, "_ZN12CPointLights8AddLightEh7CVectorS0_ffffhbP7CEntity"));
    
    HOOK(ProcessGame, aml->GetSym(hGTASA, "_ZN5CGame7ProcessEv"));
}
