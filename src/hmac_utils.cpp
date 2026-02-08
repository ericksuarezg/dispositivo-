#include "hmac_utils.h"
#include "mbedtls/md.h"

//static const char* SECRET_KEY = "AGUISU_ARMONIA_KEY_2026";
static const char* SECRET_KEY = "AGUISU_CORE_KEY_2026";

String generateHMAC(const String& payload) {
    uint8_t hmacResult[32];
    char hmacHex[65];

    // Usar solo la clave secreta, igual que Node.js
    String deviceKey = String(SECRET_KEY);

    mbedtls_md_context_t ctx;
    const mbedtls_md_info_t* info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);

    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, info, 1);
    mbedtls_md_hmac_starts(&ctx,
        (const unsigned char*)deviceKey.c_str(),
        deviceKey.length());
    mbedtls_md_hmac_update(&ctx,
        (const unsigned char*)payload.c_str(),
        payload.length());
    mbedtls_md_hmac_finish(&ctx, hmacResult);
    mbedtls_md_free(&ctx);
    Serial.println("Clave usada para HMAC:");
    Serial.println(deviceKey);

    for (int i = 0; i < 32; i++) {
        sprintf(&hmacHex[i * 2], "%02x", hmacResult[i]);
    }

    return String(hmacHex);
}
 