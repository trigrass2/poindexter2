#ifndef __SLAVE_REGISTERS_H__
#define __SLAVE_REGISTERS_H__

// A list of slave registers and their offsets
#define SLAVE_TYPE          		0x0
#define SLAVE_TYPE_SIZE     		1

#define SLAVE_REVISION      		0x1
#define SLAVE_REVISION_SIZE 		1

#define SLAVE_BUILD         		0x2
#define SLAVE_BUILD_SIZE        	2

#define SLAVE_SUPPORTED_FMMU    	0x4
#define SLAVE_SUPPORTED_FMMU_SIZE 	1

#define SLAVE_SUPPORTED_SM			0x5
#define SLAVE_SUPPORTED_SM_SIZE		1

#define SLAVE_RAM_SIZE				0x6
#define SLAVE_RAM_SIZE_SIZE			1

#define SLAVE_PORT_DESCRIPTOR		0x7
#define SLAVE_PORT_DESCRIPTOR_SIZE  1

#define SLAVE_ESC_FEATURES			0x8
#define SLAVE_ESC_FEATURES_SIZE		2
// Gap?
#define SLAVE_STATION_ADDR			0x10
#define SLAVE_STATION_ADDR_SIZE		2

// Station alias is ignored by default (ESC_DL_CONTROL bit 24)
#define SLAVE_STATION_ALIAS			0x12
#define SLAVE_STATION_ALIAS_SIZE	2

#define SLAVE_WRITE_REG_ENABLE		0x20
#define SLAVE_WRITE_REG_ENABLE_SIZE	1

#define SLAVE_WRITE_REG_PROT		0x21
#define SLAVE_WRITE_REG_PROT_SIZE	1

#define SLAVE_WRITE_ESC_ENABLE		0x30
#define SLAVE_WRITE_ESC_ENABLE_SIZE	1

#define SLAVE_WRITE_ESC_PROT		0x31
#define SLAVE_WRITE_ESC_PROT_SIZE	1

#define SLAVE_ESC_RESET_ECAT		0x40
#define SLAVE_ESC_RESET_ECAT_SIZE	1

#define SLAVE_ESC_RESET_PDI			0x41
#define SLAVE_ESC_RESET_PDI_SIZE	1

#define SLAVE_PDI_CONTROL           0x140
#define SLAVE_PDI_CONTROL_SIZE      1

#define SLAVE_EEPROM_CONTROL		0x502
#define SLAVE_EEPROM_CONTROL_SIZE	2
#define SLAVE_EEPROM_CONTROL_COMMAND_MASK   (0x7 << 8)
#define SLAVE_EEPROM_CONTROL_COMMAND_IDLE   (0 << 8)
#define SLAVE_EEPROM_CONTROL_COMMAND_READ   (1 << 8)
#define SLAVE_EEPROM_CONTROL_COMMAND_WRITE  (2 << 8)
#define SLAVE_EEPROM_CONTROL_COMMAND_RELOAD (4 << 8)

#define SLAVE_EEPROM_ADDRESS		0x504
#define SLAVE_EEPROM_ADDRESS_SIZE	4

// TODO: Make this configurable. It's 4 bytes on ESC20, 8 on others.
#define SLAVE_EEPROM_DATA			0x508
#define SLAVE_EEPROM_DATA_SIZE		4

#endif