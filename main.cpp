#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>
#include <unordered_set>
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
    {1240, PickupColors::Health},{1242, PickupColors::Armour},{1274, PickupColors::Yellow},
    {1247, PickupColors::Yellow},{1272, PickupColors::PropertyLocked},{1273, PickupColors::PropertySale},
    {326, PickupColors::Melee},{331, PickupColors::Melee},{333, PickupColors::Melee},
    {335, PickupColors::Melee},{338, PickupColors::Melee},{339, PickupColors::Melee},
    {341, PickupColors::Melee},{334, PickupColors::Melee},{321, PickupColors::Gift},
    {322, PickupColors::Gift},{323, PickupColors::Gift},{324, PickupColors::Gift},
    {325, PickupColors::Gift},{337, PickupColors::Melee},{336, PickupColors::Melee},
    {346, PickupColors::Handgun},{347, PickupColors::Handgun},{348, PickupColors::Handgun},
    {349, PickupColors::Shotgun},{350, PickupColors::Shotgun},{351, PickupColors::Shotgun},
    {355, PickupColors::Assault},{356, PickupColors::Assault},{342, PickupColors::Throwable},
    {343, PickupColors::Throwable},{344, PickupColors::Throwable},{359, PickupColors::Heavy},
    {360, PickupColors::Heavy},{361, PickupColors::Heavy},{362, PickupColors::Heavy},
    {363, PickupColors::Throwable},{365, PickupColors::Special1},{367, PickupColors::Special1},
    {368, PickupColors::Special1},{369, PickupColors::Special1},{370, PickupColors::Special1},
    {366, PickupColors::Misc},{371, PickupColors::Misc},{1275, PickupColors::PropertyLocked},
};
std::unordered_set<uint16_t> PickupsOnlyCentered {
    1239, 1240, 1241, 1247, 1253, 1254, 1313, 1274, 1275, 1272, 1273, 1314, 1277
};
inline bool IsCenteredOnly(uint16_t modelIdx)
{
    return PickupsOnlyCentered.find(modelIdx) != PickupsOnlyCentered.end();
}

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
static const uint8_t PickupCoronaIntensity = 128;
static const uint8_t PickupOuterCoronaIntensity = 96;
static const uint8_t PickupInnerCoronaIntensity = 128;

inline void DoPickupGlowing(CPickup* pu)
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
                
                RegisterCorona((unsigned int)(pu->m_pObject), NULL, intensity, intensity, intensity, PickupCoronaIntensity, pu->m_pObject->GetPosition(),
                               0.6f, 40.0f, CORONATYPE_TORUS, FLARETYPE_NONE, false, false, 0, 0.0f, false, 1.5f, 0, 15.0f, false, true);
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
                
                RegisterCorona((unsigned int)(pu->m_pObject), NULL, 0, intensity, 0, PickupCoronaIntensity, pu->m_pObject->GetPosition(),
                               0.25f, 40.0f, CORONATYPE_TORUS, FLARETYPE_NONE, false, false, 0, 0.0f, false, 1.5f, 0, 15.0f, false, true);
                StoreStaticShadow((unsigned int)(pu->m_pObject), 2, *gpShadowExplosionTex, &pu->m_pObject->GetPosition(), 2.0f, 0, 0, -2.0f, 0x10,
                                  0, intensity, 0, 4.0f, 1.0f, 40.0f, false, 0.0f);
            }
            break;
        }
        
        default:
        {
            if(pu->m_nPickupType == PICKUP_NONE) break;
            
            unsigned int asId = (unsigned int)(pu->m_pObject);
            CRGBA clr = PickupColors::FindFor(pu->m_pObject->m_nModelIndex);
            if(IsCenteredOnly(pu->m_pObject->m_nModelIndex))
            {
                RegisterCorona(asId + 9, NULL, (uint8_t)(clr.r * 0.495f), (uint8_t)(clr.g * 0.495f), (uint8_t)(clr.b * 0.495f),
                               PickupCoronaIntensity, pu->m_pObject->GetPosition(), 1.2f, 50.0f, CORONATYPE_SHINYSTAR, FLARETYPE_NONE, true, false, 1, 0.0f, false, 1.5f, 0, 15.0f, false, true);
                break;
            }
            
            bool glowInner, glowOuter;
            
            uint8_t glowStage1 = (uint8_t)((*m_snTimeInMilliseconds >> 9) & 7);
            uint8_t glowStage2 = (glowStage1 - 1) & 7;
            
            if(glowStage1 == 3) glowOuter = (rand() & 3) != 0;
            else if(glowStage1 > 3) glowOuter = true;
            
            if(glowStage2 == 3) glowInner = (rand() & 3) != 0;
            else if(glowStage2 > 3) glowInner = true;
            
            if(glowOuter)
            {
                RegisterCorona(asId, NULL, (uint8_t)(clr.r * 0.45f), (uint8_t)(clr.g * 0.45f), (uint8_t)(clr.b * 0.45f), PickupOuterCoronaIntensity, pu->m_pObject->GetPosition(),
                               0.76f, 65.0f, CORONATYPE_TORUS, FLARETYPE_NONE, false, false, 0, 0.0f, false, -0.4f, 0, 15.0f, false, true);
                StoreStaticShadow(asId, 2, *gpShadowExplosionTex, &pu->m_pObject->GetPosition(), 2.0f, 0, 0, -2.0f, 0x10,
                                  (uint8_t)(clr.r * 0.2f), (uint8_t)(clr.g * 0.2f), (uint8_t)(clr.b * 0.2f), 4.0f, 1.0f, 40.0f, false, 0.0f);
                                  
                float lightRange = (rand() % 0xF) * 0.1f + 3.0f;
                AddLight(0, pu->m_pObject->GetPosition(), CVector(), lightRange, (uint8_t)(clr.r * 0.0039f), (uint8_t)(clr.g * 0.0039f), (uint8_t)(clr.b * 0.0039f), 0, true, NULL);
            }
            else
            {
                RegisterCorona(asId, NULL, 0, 0, 0, PickupOuterCoronaIntensity, pu->m_pObject->GetPosition(),
                               0.76f, 65.0f, CORONATYPE_TORUS, FLARETYPE_NONE, false, false, 0, 0.0f, false, -0.4f, 0, 15.0f, false, true);
            }
            
            if(glowInner)
            {
                RegisterCorona(asId+1, NULL, (uint8_t)(clr.r * 0.45f), (uint8_t)(clr.g * 0.45f), (uint8_t)(clr.b * 0.45f), PickupInnerCoronaIntensity, pu->m_pObject->GetPosition(),
                               0.6f, 65.0f, CORONATYPE_TORUS, FLARETYPE_NONE, false, false, 0, 0.0f, false, -0.4f, 0, 15.0f, false, true);
                StoreStaticShadow(asId, 2, *gpShadowExplosionTex, &pu->m_pObject->GetPosition(), 2.0f, 0, 0, -2.0f, 0x10,
                                  (uint8_t)(clr.r * 0.2f), (uint8_t)(clr.g * 0.2f), (uint8_t)(clr.b * 0.2f), 4.0f, 1.0f, 40.0f, false, 0.0f);
            }
            else
            {
                RegisterCorona(asId+1, NULL, 0, 0, 0, PickupInnerCoronaIntensity, pu->m_pObject->GetPosition(),
                               0.6f, 65.0f, CORONATYPE_TORUS, FLARETYPE_NONE, false, false, 0, 0.0f, false, -0.4f, 0, 15.0f, false, true);
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
