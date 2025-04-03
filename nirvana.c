#define NIRVANA_IMPL
#define NIRVANA_ENABLE_LOG

#include "nirvana.h"

#define PSN00B_INCLUDE_DIR	"/usr/local/include/libpsn00b"
#define PSN00B_LIB_DIR		"/usr/local/lib/libpsn00b/"
#define PSN00B_LIB_RL_DIR	PSN00B_LIB_DIR"release/"
#define PSN00B_LIB_DB_DIR	PSN00B_LIB_DIR"debug/"

#define ARTIFACTS_DIR		"artifacts/"
#define SRC_NAME		"main.c"
#define OUTPUT_NAME		"game"


#define CC			"mipsel-none-elf-gcc"
#define CFLAGS			"-g -g -Wa,--strip-local-absolute -ffreestanding -fno-builtin -nostdlib "	\
                                "-fdata-sections -ffunction-sections -fsigned-char -fno-strict-overflow "	\
                                "-fdiagnostics-color=always -msoft-float -march=r3000 -mtune=r3000 -mabi=32 "	\
                                "-mno-mt -mno-llsc -Og -mdivide-breaks -G8 -fno-pic -mno-abicalls -mgpopt "	\
                                "-mno-extern-sdata"

#define LDFLAGS			PSN00B_LIB_DB_DIR"libc_exe_gprel.a -lgcc "					\
                                PSN00B_LIB_DB_DIR"libpsxgpu_exe_gprel.a "					\
                                PSN00B_LIB_DB_DIR"libpsxgte_exe_gprel.a "					\
                                PSN00B_LIB_DB_DIR"libpsxspu_exe_gprel.a "					\
                                PSN00B_LIB_DB_DIR"libpsxcd_exe_gprel.a "					\
                                PSN00B_LIB_DB_DIR"libpsxpress_exe_gprel.a "					\
                                PSN00B_LIB_DB_DIR"libpsxsio_exe_gprel.a "					\
                                PSN00B_LIB_DB_DIR"libpsxetc_exe_gprel.a "					\
                                PSN00B_LIB_DB_DIR"libpsxapi_exe_gprel.a "					\
                                PSN00B_LIB_DB_DIR"libsmd_exe_gprel.a "						\
                                PSN00B_LIB_DB_DIR"liblzp_exe_gprel.a "						\
                                PSN00B_LIB_DB_DIR"libc_exe_gprel.a"						\
                                                                

#define ELF2X			"elf2x"
#define NM			"mipsel-none-elf-nm"

#define TIM_ASM			"tim.s"

static void run_build_obj_and_dep(nir_cmd_t *cmd) {
    NIR_INFO("Building object and dep files ...\n");
    nir_cmd_append(cmd, CC);
    nir_cmd_append(cmd, CFLAGS);
    nir_cmd_append(cmd, "-MD -MT "ARTIFACTS_DIR OUTPUT_NAME".o");
    nir_cmd_append(cmd, "-MF "ARTIFACTS_DIR SRC_NAME".dep");
    nir_cmd_append(cmd, "-I"PSN00B_INCLUDE_DIR);
    nir_cmd_append(cmd, "-o "ARTIFACTS_DIR SRC_NAME".o");
    nir_cmd_append(cmd, "-c "SRC_NAME);
    
    nir_cmd_run(cmd);
}

static void run_build_tims(nir_cmd_t *cmd) {
    NIR_INFO("Building TIM assembly source files ...\n");
    nir_cmd_append(cmd, CC);
    nir_cmd_append(cmd, CFLAGS);
    nir_cmd_append(cmd, "-o "ARTIFACTS_DIR TIM_ASM".o");
    nir_cmd_append(cmd, "-c "TIM_ASM);

    nir_cmd_run(cmd);
}


static void run_link_obj(nir_cmd_t *cmd) {
    NIR_INFO("Linking object files ...\n");
    nir_cmd_append(cmd, CC);
    nir_cmd_append(cmd, "-g");
    nir_cmd_append(cmd, "-T"PSN00B_LIB_DIR"ldscripts/exe.ld");
    nir_cmd_append(cmd, "-nostdlib -Wl,-gc-sections -G8 -static");
    nir_cmd_append(cmd, ARTIFACTS_DIR SRC_NAME".o");
    nir_cmd_append(cmd, ARTIFACTS_DIR TIM_ASM".o");
    nir_cmd_append(cmd, "-o "OUTPUT_NAME".elf");
    nir_cmd_append(cmd, LDFLAGS);
    
    nir_cmd_run(cmd);
}

static void run_build_exe(nir_cmd_t *cmd) {
    NIR_INFO("Building exe ...\n");
    nir_cmd_append(cmd, ELF2X);
    nir_cmd_append(cmd, "-q "OUTPUT_NAME".elf " OUTPUT_NAME".exe");
    
    nir_cmd_run(cmd);
}

static void run_build_map(nir_cmd_t *cmd) {
    NIR_INFO("Building map file ...\n");
    nir_cmd_append(cmd, NM);
    nir_cmd_append(cmd, "-f posix -l -n "OUTPUT_NAME".elf");
    nir_cmd_append(cmd, "> "ARTIFACTS_DIR OUTPUT_NAME".map");
    
    nir_cmd_run(cmd);
}

int main(void) {
    nir_cmd_goes_samsara();
    nir_cmd_t cmd = {0};
    if (!nir_cmd_init(&cmd)) {
        return -1;
    }
    
    run_build_tims(&cmd);    
    run_build_obj_and_dep(&cmd);    
    run_link_obj(&cmd);
    run_build_exe(&cmd);
    run_build_map(&cmd);

    nir_cmd_cleanup(&cmd);
    return 0;
}