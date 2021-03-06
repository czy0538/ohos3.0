/*
 * Copyright (C) 2021 HiSilicon (Shanghai) Technologies CO., LIMITED.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "drv_osal_lib.h"

#ifndef __HuaweiLite__
#ifdef IRQ_DTS_SUPPORT
#include <linux/of_platform.h>

static int hi35xx_cipher_probe(struct platform_device *pdev)
{
    hi_s32 irq_num;
    hi_u32 rsa_int_valid = 0;
    hi_u32 rsa_int_num = 0;

    module_get_attr(CRYPTO_MODULE_ID_IFEP_RSA, &rsa_int_valid, &rsa_int_num, HI_NULL);

#if defined(ARCH_TYPE_amp) && !defined(CRYPTO_SEC_CPU)
    irq_num = platform_get_irq_byname(pdev, "nonsec_cipher");
    if (irq_num <= 0) {
        dev_err(&pdev->dev, "cannot find cipher IRQ number %d.\n", irq_num);
        return HI_ERR_CIPHER_FAILED_INIT;
    }
    module_set_irq(CRYPTO_MODULE_ID_SYMC, irq_num);

    irq_num = platform_get_irq_byname(pdev, "nonsec_hash");
    if (irq_num <= 0) {
        dev_err(&pdev->dev, "cannot find trng IRQ number %d.\n", irq_num);
        return HI_ERR_CIPHER_FAILED_INIT;
    }
    module_set_irq(CRYPTO_MODULE_ID_HASH, irq_num);

    if (rsa_int_valid == 1) {
        irq_num = platform_get_irq_byname(pdev, "nonsec_rsa");
        if (irq_num <= 0) {
            dev_err(&pdev->dev, "cannot find rsa IRQ number %d.\n", irq_num);
            return HI_ERR_CIPHER_FAILED_INIT;
        }
        module_set_irq(CRYPTO_MODULE_ID_IFEP_RSA, irq_num);
    }
#else
    irq_num = platform_get_irq_byname(pdev, "cipher");
    if (irq_num <= 0) {
        dev_err(&pdev->dev, "cannot find cipher IRQ number %d.\n", irq_num);
        return HI_ERR_CIPHER_FAILED_INIT;
    }
    module_set_irq(CRYPTO_MODULE_ID_SYMC, irq_num);

    irq_num = platform_get_irq_byname(pdev, "hash");
    if (irq_num <= 0) {
        dev_err(&pdev->dev, "cannot find trng IRQ number %d.\n", irq_num);
        return HI_ERR_CIPHER_FAILED_INIT;
    }
    module_set_irq(CRYPTO_MODULE_ID_HASH, irq_num);

    if (rsa_int_valid == 1) {
        irq_num = platform_get_irq_byname(pdev, "rsa");
        if (irq_num <= 0) {
            dev_err(&pdev->dev, "cannot find rsa IRQ number %d.\n", irq_num);
            return HI_ERR_CIPHER_FAILED_INIT;
        }
        module_set_irq(CRYPTO_MODULE_ID_IFEP_RSA, irq_num);
    }
#endif

    return cipher_drv_mod_init();
}

static int hi35xx_cipher_remove(struct platform_device *pdev)
{
    crypto_unused(pdev);
    cipher_drv_mod_exit();

    return 0;
}

static const struct of_device_id g_hi35xx_cipher_match[] = {
    { .compatible = "hisilicon,hisi-cipher" },
    { },
};
MODULE_DEVICE_TABLE(of, g_hi35xx_cipher_match);

static struct platform_driver g_hi35xx_cipher_driver = {
    .probe          = hi35xx_cipher_probe,
    .remove         = hi35xx_cipher_remove,
    .driver         = {
        .name   = "hi35xx_cipher",
        .of_match_table = g_hi35xx_cipher_match,
    },
};

module_platform_driver(g_hi35xx_cipher_driver);
MODULE_LICENSE("GPL");
#else
static int __init cipher_mod_init(void)
{
    return cipher_drv_mod_init();
}

static void __exit cipher_mod_exit(void)
{
    cipher_drv_mod_exit();
}

module_init(cipher_mod_init);
module_exit(cipher_mod_exit);

MODULE_LICENSE("GPL");
#endif
#endif
