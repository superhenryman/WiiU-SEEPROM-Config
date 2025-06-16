#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "offsets.h"
#include <string.h>
typedef struct SEEPROM_DATA {
    uint8_t  rng_seed[8];               // 0x012
    uint8_t  espresso_pkg_info[12];     // 0x020
    uint8_t  latte_pkg_info[12];        // 0x02C
    uint8_t  board_config[48];          // 0x038
    uint8_t  drive_key[10];             // 0x080
    uint8_t  factory_key[10];           // 0x090
    uint8_t  shdd_seed[10];             // 0x0A0
    uint8_t  ivs_seed[10];              // 0x0B0
    uint16_t drive_config;              // 0x0C0
    uint16_t ivs_config;                // 0x0C2
    uint16_t shdd_config;               // 0x0C4
    uint8_t  sys_prod_info[40];         // 0x140
    uint8_t  cpu_board_log[12];         // 0x180
    uint8_t  dcrmcr[20];                // 0x192
    uint8_t  diag_result_log[8];        // 0x1B2
    uint8_t  other_info[2];             // 0x1BE
    uint8_t  boot_params[30];           // 0x1C0
} SEEPROM_DATA;

/*SEEPROM_DATA parse_seeprom() {
    FILE* f = fopen("seeprom.bin", "rb");
    if (!f) {
        printf("No seeprom.bin found. Please dump one from nanddumper.\n");
        exit(EXIT_FAILURE);
    }
    SEEPROM_DATA seeprom;
    size_t read_bytes = fread(&seeprom, 1, sizeof(SEEPROM_DATA), f);
    if (read_bytes != sizeof(SEEPROM_DATA)) {
        fprintf(stderr, "Error reading seeprom.bin. Expected %zu bytes, got %zu bytes. Your SEEPROM dump might be bad. Or my code is bad. Whichever it is fuck me.\n", sizeof(SEEPROM_DATA), read_bytes);
        fclose(f);
        exit(EXIT_FAILURE);
    }
    fclose(f);
 
   return seeprom;
} */

void print_hex(const char* label, const uint8_t* data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; ++i) {
        printf("%02X", data[i]);
        if ((i + 1) < len)
            printf(" ");
    }
    printf("\n");
}

void print_seeprom_info(const SEEPROM_DATA* s) {
    print_hex("RNG Seed", s->rng_seed, sizeof(s->rng_seed));
    print_hex("Espresso Package Info", s->espresso_pkg_info, sizeof(s->espresso_pkg_info));
    print_hex("Latte Package Info", s->latte_pkg_info, sizeof(s->latte_pkg_info));
    print_hex("Board Config", s->board_config, sizeof(s->board_config));
    print_hex("Drive Key", s->drive_key, sizeof(s->drive_key));
    print_hex("Factory Key", s->factory_key, sizeof(s->factory_key));
    print_hex("SHDD Seed", s->shdd_seed, sizeof(s->shdd_seed));
    print_hex("IVS Seed", s->ivs_seed, sizeof(s->ivs_seed));

    printf("Drive Config: 0x%04X\n", s->drive_config);
    printf("IVS Config:   0x%04X\n", s->ivs_config);
    printf("SHDD Config:  0x%04X\n", s->shdd_config);

    print_hex("System Product Info", s->sys_prod_info, sizeof(s->sys_prod_info));
    print_hex("CPU Board Log", s->cpu_board_log, sizeof(s->cpu_board_log));
    print_hex("DCR/MCR", s->dcrmcr, sizeof(s->dcrmcr));
    print_hex("Diag Result Log", s->diag_result_log, sizeof(s->diag_result_log));
    print_hex("Other Info", s->other_info, sizeof(s->other_info));
    print_hex("Boot Params", s->boot_params, sizeof(s->boot_params));
}



int read_section(FILE* f, long offset, size_t size, uint8_t* buffer) {
    if (fseek(f, offset, SEEK_SET) != 0) {
        fprintf(stderr, "Error: could not seek to offset 0x%lX\n", offset);
        return 1;
    }
    size_t read_bytes = fread(buffer, 1, size, f);
    if (read_bytes != size) {
        fprintf(stderr, "Error: expected %zu bytes but read %zu bytes at offset 0x%lX\n", size, read_bytes, offset);
        return 2;
    }
    return 0;
}

int main() {
    FILE* f = fopen("seeprom.bin", "rb");
    SEEPROM_DATA seeprom;
    if (!f) {
        fprintf(stderr, "Error: could not open seeprom.bin\n");
        fprintf(stderr, "Please dump one from nanddumper or name your seeprom to exactly: 'seeprom.bin'\n");

        return EXIT_FAILURE;
    }
    uint8_t rng_seed[RNG_SEED_SIZE];
    uint8_t espresso_pkg_info[ESPRESSO_PKG_INFO_SIZE];
    uint8_t latte_pkg_info[LATTE_PKG_INFO_SIZE];
    uint8_t board_config[BOARD_CONFIG_SIZE];
    uint8_t drive_key[DRIVE_KEY_SIZE]; // clear
    uint8_t factory_key[FACTORY_KEY_SIZE];
    uint8_t shdd_seed[SHDD_SEED_SIZE];
    uint8_t ivs_seed[IVS_SEED_SIZE]; // clear
    uint16_t drive_config;
    uint16_t ivs_config;
    uint16_t shdd_config;
    uint8_t sys_prod_info[SYS_PROD_INFO_SIZE];
    uint8_t cpu_board_log[CPU_BOARD_LOG_SIZE];
    uint8_t dcrmcr[DCRMCR_SIZE];
    uint8_t diag_result_log[DIAG_RESULT_LOG_SIZE];
    uint8_t other_info[OTHER_INFO_SIZE];
    uint8_t boot_params[BOOT_PARAMS_SIZE];
    // HOLY SHIT THIS IS A HUGE IF STATEMENT
    if (read_section(f, RNG_SEED_OFFSET, RNG_SEED_SIZE, rng_seed) != 0 || read_section(f, ESPRESSO_PKG_INFO_OFFSET, ESPRESSO_PKG_INFO_SIZE, espresso_pkg_info) != 0 ||
        read_section(f, LATTE_PKG_INFO_OFFSET, LATTE_PKG_INFO_SIZE, latte_pkg_info) != 0 || read_section(f, BOARD_CONFIG_OFFSET, BOARD_CONFIG_SIZE, board_config) != 0 ||
        read_section(f, DRIVE_KEY_OFFSET, DRIVE_KEY_SIZE, drive_key) != 0 || read_section(f, FACTORY_KEY_OFFSET, FACTORY_KEY_SIZE, factory_key) != 0 ||
        read_section(f, SHDD_SEED_OFFSET, SHDD_SEED_SIZE, shdd_seed) != 0 || read_section(f, IVS_SEED_OFFSET, IVS_SEED_SIZE, ivs_seed) != 0 ||
        read_section(f, DRIVE_CONFIG_OFFSET, DRIVE_CONFIG_SIZE, (uint8_t*)&drive_config) != 0 || read_section(f, IVS_CONFIG_OFFSET, IVS_CONFIG_SIZE, (uint8_t*)&ivs_config) != 0 ||
        read_section(f, SHDD_CONFIG_OFFSET, SHDD_CONFIG_SIZE, (uint8_t*)&shdd_config) != 0 || read_section(f, SYS_PROD_INFO_OFFSET, SYS_PROD_INFO_SIZE, sys_prod_info) != 0 ||
        read_section(f, CPU_BOARD_LOG_OFFSET, CPU_BOARD_LOG_SIZE, cpu_board_log) != 0 || read_section(f, DCRMCR_OFFSET, DCRMCR_SIZE, dcrmcr) != 0 ||
        read_section(f, DIAG_RESULT_LOG_OFFSET, DIAG_RESULT_LOG_SIZE, diag_result_log) != 0 || read_section(f, OTHER_INFO_OFFSET, OTHER_INFO_SIZE, other_info) != 0 ||
        read_section(f, BOOT_PARAMS_OFFSET, BOOT_PARAMS_SIZE, boot_params) != 0) {
        fprintf(stderr, "Error: failed to read one or more sections from seeprom.bin\n");
        fclose(f);
        return EXIT_FAILURE;
    }
    memcpy(seeprom.rng_seed, rng_seed, sizeof(seeprom.rng_seed));
    memcpy(seeprom.espresso_pkg_info, espresso_pkg_info, sizeof(seeprom.espresso_pkg_info));
    memcpy(seeprom.latte_pkg_info, latte_pkg_info, sizeof(seeprom.latte_pkg_info));
    memcpy(seeprom.board_config, board_config, sizeof(seeprom.board_config));
    memcpy(seeprom.drive_key, drive_key, sizeof(seeprom.drive_key));
    memcpy(seeprom.factory_key, factory_key, sizeof(seeprom.factory_key));
    memcpy(seeprom.shdd_seed, shdd_seed, sizeof(seeprom.shdd_seed));
    memcpy(seeprom.ivs_seed, ivs_seed, sizeof(seeprom.ivs_seed));
    seeprom.drive_config = drive_config;
    seeprom.ivs_config = ivs_config;
    seeprom.shdd_config = shdd_config;
    memcpy(seeprom.sys_prod_info, sys_prod_info, sizeof(seeprom.sys_prod_info));
    memcpy(seeprom.cpu_board_log, cpu_board_log, sizeof(seeprom.cpu_board_log));
    memcpy(seeprom.dcrmcr, dcrmcr, sizeof(seeprom.dcrmcr));
    memcpy(seeprom.diag_result_log, diag_result_log, sizeof(seeprom.diag_result_log));
    memcpy(seeprom.other_info, other_info, sizeof(seeprom.other_info));
    memcpy(seeprom.boot_params, boot_params, sizeof(seeprom.boot_params));
    print_seeprom_info(&seeprom);
    printf("Learn more about these values at: https://wiiubrew.org/wiki/Hardware/EEPROM\n");
    printf("If you want to dump this to a file, use > seeprom_config.txt after writing the file name.\n");
    printf("If this program outputs garbage, you aren't using a seeprom.bin file from a Wii U. WII AND 3DS SEEPROMS WILL NOT OUTPUT CORRECT DATA!\n");
    fclose(f);
    return 0;
}