/*
 * Arcturus Networks Inc. uCP1020 module Setup
 *
 * Copyright 2014-2016 Arcturus Networks Inc.
 *
 * by Oleksandr G Zhadan & Michael Durrant (www.ArcturusNetworks.com)
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/stddef.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/of_platform.h>

#include <asm/time.h>
#include <asm/machdep.h>
#include <asm/pci-bridge.h>
#include <mm/mmu_decl.h>
#include <asm/prom.h>
#include <asm/udbg.h>
#include <asm/mpic.h>

#include <sysdev/fsl_soc.h>
#include <sysdev/fsl_pci.h>

#include "mpc85xx.h"

void __init ucp1020_som_pic_init(void)
{
	struct mpic *mpic = mpic_alloc(NULL, 0, MPIC_BIG_ENDIAN |
	  MPIC_SINGLE_DEST_CPU,
	  0, 256, " OpenPIC  ");

	BUG_ON(mpic == NULL);

	mpic_init(mpic);
}

/*
 * Setup the architecture
 */
static void __init ucp1020_som_setup_arch(void)
{
	if (ppc_md.progress)
		ppc_md.progress("uCP1020_SoM_setup_arch()", 0);

//	mpc85xx_smp_init();

	fsl_pci_assign_primary();
	pr_info("\n\t%s (http://www.arcturusnetworks.com)\n", ppc_md.name);
}

machine_arch_initcall(ucp1020, mpc85xx_common_publish_devices);
machine_arch_initcall(ucp1020, swiotlb_setup_bus_notifier);

/*
 * Called very early, device-tree isn't unflattened
 */
static int __init ucp1020_probe(void)
{
	unsigned long root = of_get_flat_dt_root();

	if (of_flat_dt_is_compatible(root, "arcturus,uCP1020"))
		return 1;
	return 0;
}

define_machine(ucp1020) {
	.name			= "uCP1020 SoM - Arcturus Networks Inc.",
	.probe			= ucp1020_probe,
	.setup_arch		= ucp1020_som_setup_arch,
	.init_IRQ		= ucp1020_som_pic_init,
#ifdef CONFIG_PCI
	.pcibios_fixup_bus	= fsl_pcibios_fixup_bus,
	.pcibios_fixup_phb      = fsl_pcibios_fixup_phb,
#endif
	.get_irq		= mpic_get_irq,
	.restart		= fsl_rstcr_restart,
	.calibrate_decr		= generic_calibrate_decr,
#ifdef DEBUG 
	.progress		= udbg_progress,
#endif
};
