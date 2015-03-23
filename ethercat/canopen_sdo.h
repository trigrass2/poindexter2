#ifndef __CANOPEN_SDO_H__
#define __CANOPEN_SDO_H__

// Anything without a sub-index should just default to SI 0

#define SDO_I_COE_PDO_MAPPING_0 0x1c10 // PDO mapping for SM 0
#define SDO_I_COE_PDO_MAPPING_1 0x1c11 // PDO mapping for SM 1
#define SDO_I_COE_PDO_MAPPING_2 0x1c12 // PDO mapping for SM 2
#define SDO_I_COE_PDO_MAPPING_3 0x1c13 // PDO mapping for SM 3
#define SDO_SI_COE_PDO_MAPPING_COUNT 0 // Number of mappings sub-index
#define SDO_SI_COE_PDO_MAPPING(mapping_num) (mapping_num+1)

// PLL interpolation period in synchronous mode
#define SDO_I_INTERPOLATION_PERIOD 0x60C2
#define SDO_SI_INTERPOLATION_PERIOD_UNIT 1
#define SDO_SI_INTERPOLATION_PERIOD_INDEX 2

#define SDO_I_OPERATION_MODE 0x6060

// Manufacturer specific
#define SDO_OPERATION_MODE_ELEC_GEARING    0xF7
#define SDO_OPERATION_MODE_JOGGING         0xF8
#define SDO_OPERATION_MODE_HOMING          0xF9
#define SDO_OPERATION_MODE_TRAJECTORY      0xFA
#define SDO_OPERATION_MODE_ANALOG_CURRENT  0xFB
#define SDO_OPERATION_MODE_ANALOG_SPEED    0xFC
#define SDO_OPERATION_MODE_DIGITAL_CURRENT 0xFD
#define SDO_OPERATION_MODE_DIGITAL_SPEED   0xFE
#define SDO_OPERATION_MODE_POSITION        0xFF

// CANopen profiles
#define SDO_OPERATION_MODE_DS402_POSITIONING   0x1
#define SDO_OPERATION_MODE_DS402_SPEED_VL      0x2
#define SDO_OPERATION_MODE_DS402_SPEED_PV      0x3
#define SDO_OPERATION_MODE_DS402_TORQUE        0x4
#define SDO_OPERATION_MODE_DS402_HOMING        0x6
#define SDO_OPERATION_MODE_DS402_INTERPOLATION 0x7

// Read the current operation mode...
#define SDO_I_OPERATION_MODE_ACTUAL 0x6061

// The status masks
#define CANOPEN_STATUS_SWITCH_ON_MASK          (1 << 0)
#define CANOPEN_STATUS_SWITCHED_ON_MASK        (1 << 1)
#define CANOPEN_STATUS_ENABLED_MASK            (1 << 2)
#define CANOPEN_STATUS_FAULT_MASK              (1 << 3)
#define CANOPEN_STATUS_DISABLE_VOLTAGE_MASK    (1 << 4)
#define CANOPEN_STATUS_QUICK_STOP_MASK         (1 << 5)
#define CANOPEN_STATUS_SWITCH_ON_DISABLED_MASK (1 << 6)
#define CANOPEN_STATUS_WARNING_MASK            (1 << 7)

#define CANOPEN_CONTROL_SWITCH_ON_MASK         (1 << 0)
#define CANOPEN_CONTROL_DISABLE_VOLTAGE_MASK   (1 << 1)
#define CANOPEN_CONTROL_QUICK_STOP_MASK        (1 << 2)
#define CANOPEN_CONTROL_ENABLE_OPERATION_MASK  (1 << 3)

#endif
