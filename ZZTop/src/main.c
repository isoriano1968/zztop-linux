/*
 * MNT ZZ Top Linux
 * 
 * main.c
 * 
 * Copyright 2021 Ignacio Soriano Hernandez <isoriano@amiga-linux>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <gtk/gtk.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>

#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

GtkWidget *g_lbl_firmware;
GtkWidget *g_lbl_temperature;
GtkWidget *g_lbl_core_voltage;
GtkWidget *g_lbl_aux_voltage;

// i2c stuff
uint8_t data, addr = 0x3b, reg = 0xe0; // Read register 0xe0 of the ZZ9000 which corresponds to temp
const char *path = "/dev/i2c-0";
int file, rc;
	
static inline __s32 i2c_smbus_access(int file, char read_write, __u8 command,
                                     int size, union i2c_smbus_data *data)
{
	struct i2c_smbus_ioctl_data args;

	args.read_write = read_write;
	args.command = command;
	args.size = size;
	args.data = data;
	return ioctl(file,I2C_SMBUS,&args);
}


static inline __s32 i2c_smbus_read_byte_data(int file, __u8 command)
{
	union i2c_smbus_data data;
	if (i2c_smbus_access(file,I2C_SMBUS_READ,command,
	                     I2C_SMBUS_BYTE_DATA,&data))
		return -1;
	else
		return 0x0FF & data.byte;
}

int main(int argc, char *argv[])
{
    // Gtk stuff
    GtkBuilder      *builder; 
    GtkWidget       *window;
    

	
    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "glade/window_main.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, NULL);
    
    g_lbl_firmware = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_firmware"));
    g_lbl_temperature = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_temperature"));
	g_lbl_core_voltage = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_core_voltage"));
	g_lbl_aux_voltage= GTK_WIDGET(gtk_builder_get_object(builder, "lbl_aux_voltage"));
    g_object_unref(builder);

    gtk_widget_show(window);                
    
    file = open(path, O_RDWR);
	if (file < 0)
		err(errno, "Tried to open '%s'", path); 

	rc = ioctl(file, I2C_SLAVE, addr);
	if (rc < 0)
		err(errno, "Tried to set device address '0x%02x'", addr);

	
	
	gtk_main();
    
    return 0;
}

void on_btn_bus_test_clicked()
{
	char str_temp[10] = {0};
	
    gtk_label_set_text(GTK_LABEL(g_lbl_firmware), "No version");
    gtk_label_set_text(GTK_LABEL(g_lbl_temperature), "No version");
    gtk_label_set_text(GTK_LABEL(g_lbl_core_voltage), "No version");
    gtk_label_set_text(GTK_LABEL(g_lbl_aux_voltage), "No version");
    
    // Reading register from ZZ9000
    printf("Reading ZZ9000 i2c");
    data = i2c_smbus_read_byte_data(file, reg);
	printf("%s: device 0x%02x at address 0x%02x: 0x%02x\n", path, addr, reg, data);
	sprintf(str_temp, "%d", data);
	gtk_label_set_text(GTK_LABEL(g_lbl_temperature), str_temp);
	
    
}

// called when window is closed
void on_window_main_destroy()
{
    gtk_main_quit();
}
