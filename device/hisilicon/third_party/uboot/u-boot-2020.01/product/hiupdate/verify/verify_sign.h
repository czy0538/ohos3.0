/*
 *
 * Copyright (c) 2020 HiSilicon (Shanghai) Technologies CO., LIMITED.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef HI_UPDATE_VERIFY_SIGH_H
#define HI_UPDATE_VERIFY_SIGH_H

#define HASH_LEN    32

int calc_image_hash(const unsigned char *data, unsigned int data_len,
	unsigned char *hash, unsigned int hash_len);
int verify_sign(const unsigned char *data, unsigned int data_len,
	unsigned char *sign, unsigned int sign_len);

#endif /* HI_UPDATE_VERIFY_SIGH_H */
