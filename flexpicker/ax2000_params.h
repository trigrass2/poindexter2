#ifndef __AX2000_PARAMS_H__
#define __AX2000_PARAMS_H__

// Borrowed from Beckhoff AX2xxx.xml
// I assume these can be set to anything, but these seem to be sensible defaults
#define AX2000_MBOX_OUT_ADDR 0x1800
#define AX2000_MBOX_OUT_SIZE 512
#define AX2000_MBOX_IN_ADDR  0x1c00
#define AX2000_MBOX_IN_SIZE  512

#define AX2000_PDO_OUT_ADDR 0x1100
#define AX2000_PDO_OUT_SIZE 6
#define AX2000_PDO_IN_ADDR  0x1140
#define AX2000_PDO_IN_SIZE  6

#define AX2000_FMMU_OUT_PHYSADDR 0x1100
#define AX2000_FMMU_OUT_LENGTH   8

#define AX2000_FMMU_IN_PHYSADDR 0x1140
#define AX2000_FMMU_IN_LENGTH 8

#define AX2000_FMMU_LOGICALADDR 0x10000
#define AX2000_FMMU_LOGICALADDR_STRIDE 0x10000

#define AX2000_VELOCITY_IN_PDO  0x1b01   // Position, status word
#define AX2000_VELOCITY_OUT_PDO 0x1702   // Velocity, control word

#endif