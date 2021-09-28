#ifndef AHCI_H
#define AHCI_H

#include <stdint.h>
#include <stddef.h>
#include <mem/vmm.h>
#include <arch/x86_64/driver/disk/sata.h>

#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATA 1
 
#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3

#define HBA_PxCMD_ST    0x0001
#define HBA_PxCMD_FRE   0x0010
#define HBA_PxCMD_FR    0x4000
#define HBA_PxCMD_CR    0x8000
#define HBA_PxIS_TFES   (1 << 30)

typedef enum{
	FIS_TYPE_REG_H2D	= 0x27,	// Register FIS - host to device
	FIS_TYPE_REG_D2H	= 0x34,	// Register FIS - device to host
	FIS_TYPE_DMA_ACT	= 0x39,	// DMA activate FIS - device to host
	FIS_TYPE_DMA_SETUP	= 0x41,	// DMA setup FIS - bidirectional
	FIS_TYPE_DATA		= 0x46,	// Data FIS - bidirectional
	FIS_TYPE_BIST		= 0x58,	// BIST activate FIS - bidirectional
	FIS_TYPE_PIO_SETUP	= 0x5F,	// PIO setup FIS - device to host
	FIS_TYPE_DEV_BITS	= 0xA1,	// Set device bits FIS - device to host
} FIS_TYPE;

// Host to device
volatile struct fis_reg_h2d{
	uint8_t fis_type;
	uint8_t pmport:4;
	uint8_t c:1;
	uint8_t command;
	uint8_t featurel;
	uint8_t lba0;
	uint8_t lba1;
	uint8_t lba2;
	uint8_t device;
	uint8_t lba3;
	uint8_t lba4;
	uint8_t lba5;
	uint8_t featureh;
	uint8_t countl;
	uint8_t counth;
	uint8_t icc;
	uint8_t control;
	uint8_t reserved[4];
}__attribute__((packed));

typedef struct fis_reg_h2d fis_reg_h2d_t;

volatile struct fis_reg_d2h{
	uint8_t fis_type;
	uint8_t pmport:4;
	uint8_t rsv0:2;
	uint8_t i:1;
	uint8_t rsv1:1;
	uint8_t status;
	uint8_t error;
	uint8_t lba0;
	uint8_t lba1;
	uint8_t lba2;
	uint8_t device;
	uint8_t lba3;
	uint8_t lba4;
	uint8_t lba5;
	uint8_t rsv2;
	uint8_t countl;
	uint8_t counth;
	uint8_t rsv3[6];
}__attribute__((packed));

typedef struct fis_reg_d2h fis_reg_d2h_t;

volatile struct fis_data{
	uint8_t fis_type;
	uint8_t pmport:4;
	uint8_t rsv0:4;
	uint8_t rsv1[2];

	uint32_t data[1];
}__attribute__((packed));

typedef struct fis_data fis_data_t;

volatile struct fis_pio_setup{
	uint8_t fis_type;
	uint8_t pmport:4;
	uint8_t rsv0:1;
	uint8_t d:1; // transfer direction 1 = device to host
	uint8_t i:1;
	uint8_t rsv1:1;
	uint8_t status;
	uint8_t error;
	uint8_t lba0;
	uint8_t lba1;
	uint8_t lba2;
	uint8_t device;
	uint8_t lba3;
	uint8_t lba4;
	uint8_t lba5;
	uint8_t rsv2;
	uint8_t countl;
	uint8_t counth;
	uint8_t rsv3;
	uint8_t e_status;
	uint16_t tc;
	uint8_t rsv4[2];
}__attribute__((packed));

typedef struct fis_pio_setup fis_pio_setup_t;

volatile struct fis_dma_setup{
	uint8_t fis_type;
	uint8_t pmport:4;
	uint8_t rsv0:1;
	uint8_t d:1; // transfer direction 1 = device to host
	uint8_t i:1;
	uint8_t a:1;
	uint8_t rsv1[2];
	uint64_t dma_buffer_id;
	uint32_t rsv2;
	uint32_t dma_buffer_offset;
	uint32_t dma_count;
	uint32_t rsv3;
}__attribute__((packed));

typedef struct fis_dma_setup fis_dma_setup_t;

volatile struct hba_port{
	uint32_t clb; // Command list base address (upper and lower)
	uint32_t clbu;
	uint32_t fb; // FIS base address (upper and lower)
	uint32_t fbu;
	volatile uint32_t is;
	uint32_t ie;
	volatile uint32_t cmd;
	uint32_t rsv0;
	volatile uint32_t tfd;
	uint32_t sig;
	uint32_t ssts;
	uint32_t sctl;
	uint32_t serr;
	uint32_t sact;
	volatile uint32_t ci;
	uint32_t sntf;
	uint32_t fbs;
	uint32_t rsv1[11];
	uint32_t vendor[4];
}__attribute__((packed));

typedef struct hba_port hba_port_t;

volatile struct hba_mem{
	uint32_t cap;
	uint32_t ghc;
	uint32_t is;
	uint32_t pi;
	uint32_t version;
	uint32_t ccc_ctl;
	uint32_t ccc_pts;
	uint32_t em_loc;
	uint32_t em_ctl;
	uint32_t cap2;
	uint32_t bohc;
	
	uint8_t reserved[116];
	uint8_t vendor[96];

	hba_port_t ports[32];
}__attribute__((packed));

volatile struct ahci_command_list{
	
}__attribute__((packed));

typedef struct ahci_command_list ahci_command_list_t;

volatile struct ahci_fis{
	fis_dma_setup_t dsfis;
	uint8_t pad0[4];
	fis_pio_setup_t psfis;
	uint8_t pad1[12];
	fis_reg_d2h_t rfis;
	uint8_t pad2[4];
	uint64_t sdbfis;
	uint8_t ufis[64];
	uint8_t rsv[96];
}__attribute__((packed));

typedef struct ahci_fis ahci_fis_t;

volatile struct hba_cmd_header{
	uint8_t cfl:5;
	uint8_t a:1;
	uint8_t w:1; // write 1=H2D
	uint8_t p:1; // prefetchable
	uint8_t r:1;
	uint8_t b:1;
	uint8_t c:1;
	uint8_t rsv0:1;
	uint8_t pmport:4;
	uint16_t prdtl;
	volatile uint32_t prdbc;
	uint32_t ctba; // command table descriptor base address (upper and lower)
	uint32_t ctbau;
	uint32_t rsv1[4];
}__attribute__((packed));

typedef struct hba_cmd_header hba_cmd_header_t;

volatile struct hba_prdt_entry{
	uint32_t dba; // Data base address (upper and lower)
	uint32_t dbau;
	uint32_t rsv0;
	uint32_t dbc:22;
	uint32_t rsv1:9;
	uint32_t i:1;
}__attribute__((packed));

typedef struct hba_prdt_entry hba_prdt_entry_t;

volatile struct hba_cmd_tbl{
	uint8_t cfis[64];
	uint8_t acmd[16];
	uint8_t rsv[48];

	hba_prdt_entry_t prdt_entry[1];
}__attribute__((packed));

typedef struct hba_cmd_tbl hba_cmd_tbl_t;

typedef struct hba_mem hba_mem_t;

void init_ahci();

#endif
