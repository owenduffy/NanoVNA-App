
#include "libusb.h"

// ****************************************

//#define t_libusb_call __cdecl
#define t_libusb_call __stdcall

typedef int                           (t_libusb_call t_libusb_init)(libusb_context **ctx);

typedef void                          (t_libusb_call t_libusb_exit)(libusb_context *ctx);

typedef void                          (t_libusb_call t_libusb_set_debug)(libusb_context *ctx, int level);
typedef void                          (t_libusb_call t_libusb_set_log_cb)(libusb_context *ctx, libusb_log_cb cb, int mode);
typedef const struct libusb_version * (t_libusb_call t_libusb_get_version)(void);
typedef int                           (t_libusb_call t_libusb_has_capability)(uint32_t capability);
typedef const char *                  (t_libusb_call t_libusb_error_name)(int errcode);
typedef int                           (t_libusb_call t_libusb_setlocale)(const char *locale);
typedef const char *                  (t_libusb_call t_libusb_strerror)(int errcode);

typedef ssize_t                       (t_libusb_call t_libusb_get_device_list)(libusb_context *ctx, libusb_device ***list);
typedef void                          (t_libusb_call t_libusb_free_device_list)(libusb_device **list, int unref_devices);
typedef libusb_device *               (t_libusb_call t_libusb_ref_device)(libusb_device *dev);
typedef void                          (t_libusb_call t_libusb_unref_device)(libusb_device *dev);

typedef int                           (t_libusb_call t_libusb_get_configuration)(libusb_device_handle *dev, int *config);
typedef int                           (t_libusb_call t_libusb_get_device_descriptor)(libusb_device *dev, struct libusb_device_descriptor *desc);
typedef int                           (t_libusb_call t_libusb_get_active_config_descriptor)(libusb_device *dev, struct libusb_config_descriptor **config);
typedef int                           (t_libusb_call t_libusb_get_config_descriptor)(libusb_device *dev, uint8_t config_index, struct libusb_config_descriptor **config);
typedef int                           (t_libusb_call t_libusb_get_config_descriptor_by_value)(libusb_device *dev, uint8_t bConfigurationValue, struct libusb_config_descriptor **config);
typedef void                          (t_libusb_call t_libusb_free_config_descriptor)(struct libusb_config_descriptor *config);
typedef int                           (t_libusb_call t_libusb_get_ss_endpoint_companion_descriptor)(struct libusb_context *ctx, const struct libusb_endpoint_descriptor *endpoint, struct libusb_ss_endpoint_companion_descriptor **ep_comp);
typedef void                          (t_libusb_call t_libusb_free_ss_endpoint_companion_descriptor)(struct libusb_ss_endpoint_companion_descriptor *ep_comp);
typedef int                           (t_libusb_call t_libusb_get_bos_descriptor)(libusb_device_handle *dev_handle, struct libusb_bos_descriptor **bos);
typedef void                          (t_libusb_call t_libusb_free_bos_descriptor)(struct libusb_bos_descriptor *bos);
typedef int                           (t_libusb_call t_libusb_get_usb_2_0_extension_descriptor)(struct libusb_context *ctx, struct libusb_bos_dev_capability_descriptor *dev_cap, struct libusb_usb_2_0_extension_descriptor **usb_2_0_extension);
typedef void                          (t_libusb_call t_libusb_free_usb_2_0_extension_descriptor)(struct libusb_usb_2_0_extension_descriptor *usb_2_0_extension);
typedef int                           (t_libusb_call t_libusb_get_ss_usb_device_capability_descriptor)(struct libusb_context *ctx, struct libusb_bos_dev_capability_descriptor *dev_cap, struct libusb_ss_usb_device_capability_descriptor **ss_usb_device_cap);
typedef void                          (t_libusb_call t_libusb_free_ss_usb_device_capability_descriptor)(struct libusb_ss_usb_device_capability_descriptor *ss_usb_device_cap);
typedef int                           (t_libusb_call t_libusb_get_container_id_descriptor)(struct libusb_context *ctx, struct libusb_bos_dev_capability_descriptor *dev_cap, struct libusb_container_id_descriptor **container_id);
typedef void                          (t_libusb_call t_libusb_free_container_id_descriptor)(struct libusb_container_id_descriptor *container_id);
typedef uint8_t                       (t_libusb_call t_libusb_get_bus_number)(libusb_device *dev);
typedef uint8_t                       (t_libusb_call t_libusb_get_port_number)(libusb_device *dev);
typedef int                           (t_libusb_call t_libusb_get_port_numbers)(libusb_device *dev, uint8_t *port_numbers, int port_numbers_len);

typedef int                           (t_libusb_call t_libusb_get_port_path)(libusb_context *ctx, libusb_device *dev, uint8_t *path, uint8_t path_length);
typedef libusb_device *               (t_libusb_call t_libusb_get_parent)(libusb_device *dev);
typedef uint8_t                       (t_libusb_call t_libusb_get_device_address)(libusb_device *dev);
typedef int                           (t_libusb_call t_libusb_get_device_speed)(libusb_device *dev);
typedef int                           (t_libusb_call t_libusb_get_max_packet_size)(libusb_device *dev, unsigned char endpoint);
typedef int                           (t_libusb_call t_libusb_get_max_iso_packet_size)(libusb_device *dev, unsigned char endpoint);

typedef int                           (t_libusb_call t_libusb_wrap_sys_device)(libusb_context *ctx, intptr_t sys_dev, libusb_device_handle **dev_handle);
typedef int                           (t_libusb_call t_libusb_open)(libusb_device *dev, libusb_device_handle **dev_handle);
typedef void                          (t_libusb_call t_libusb_close)(libusb_device_handle *dev_handle);
typedef libusb_device *               (t_libusb_call t_libusb_get_device)(libusb_device_handle *dev_handle);

typedef int                           (t_libusb_call t_libusb_set_configuration)(libusb_device_handle *dev_handle, int configuration);
typedef int                           (t_libusb_call t_libusb_claim_interface)(libusb_device_handle *dev_handle, int interface_number);
typedef int                           (t_libusb_call t_libusb_release_interface)(libusb_device_handle *dev_handle, int interface_number);

typedef libusb_device_handle *        (t_libusb_call t_libusb_open_device_with_vid_pid)(libusb_context *ctx, uint16_t vendor_id, uint16_t product_id);

typedef int                           (t_libusb_call t_libusb_set_interface_alt_setting)(libusb_device_handle *dev_handle, int interface_number, int alternate_setting);
typedef int                           (t_libusb_call t_libusb_clear_halt)(libusb_device_handle *dev_handle, unsigned char endpoint);
typedef int                           (t_libusb_call t_libusb_reset_device)(libusb_device_handle *dev_handle);

typedef int                           (t_libusb_call t_libusb_alloc_streams)(libusb_device_handle *dev_handle, uint32_t num_streams, unsigned char *endpoints, int num_endpoints);
typedef int                           (t_libusb_call t_libusb_free_streams)(libusb_device_handle *dev_handle, unsigned char *endpoints, int num_endpoints);

typedef unsigned char *               (t_libusb_call t_libusb_dev_mem_alloc)(libusb_device_handle *dev_handle, size_t length);
typedef int                           (t_libusb_call t_libusb_dev_mem_free)(libusb_device_handle *dev_handle, unsigned char *buffer, size_t length);

typedef int                           (t_libusb_call t_libusb_kernel_driver_active)(libusb_device_handle *dev_handle, int interface_number);
typedef int                           (t_libusb_call t_libusb_detach_kernel_driver)(libusb_device_handle *dev_handle, int interface_number);
typedef int                           (t_libusb_call t_libusb_attach_kernel_driver)(libusb_device_handle *dev_handle, int interface_number);
typedef int                           (t_libusb_call t_libusb_set_auto_detach_kernel_driver)(libusb_device_handle *dev_handle, int enable);

typedef struct libusb_transfer *      (t_libusb_call t_libusb_alloc_transfer)(int iso_packets);
typedef int                           (t_libusb_call t_libusb_submit_transfer)(struct libusb_transfer *transfer);
typedef int                           (t_libusb_call t_libusb_cancel_transfer)(struct libusb_transfer *transfer);
typedef void                          (t_libusb_call t_libusb_free_transfer)(struct libusb_transfer *transfer);
typedef void                          (t_libusb_call t_libusb_transfer_set_stream_id)(struct libusb_transfer *transfer, uint32_t stream_id);
typedef uint32_t                      (t_libusb_call t_libusb_transfer_get_stream_id)(struct libusb_transfer *transfer);

typedef int                           (t_libusb_call t_libusb_control_transfer)(libusb_device_handle *dev_handle, uint8_t request_type, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, unsigned char *data, uint16_t wLength, unsigned int timeout);
typedef int                           (t_libusb_call t_libusb_bulk_transfer)(libusb_device_handle *dev_handle, unsigned char endpoint, unsigned char *data, int length, int *actual_length, unsigned int timeout);
typedef int                           (t_libusb_call t_libusb_interrupt_transfer)(libusb_device_handle *dev_handle, unsigned char endpoint, unsigned char *data, int length, int *actual_length, unsigned int timeout);

typedef int                           (t_libusb_call t_libusb_get_string_descriptor_ascii)(libusb_device_handle *dev_handle, uint8_t desc_index, unsigned char *data, int length);

typedef int                           (t_libusb_call t_libusb_try_lock_events)(libusb_context *ctx);
typedef void                          (t_libusb_call t_libusb_lock_events)(libusb_context *ctx);
typedef void                          (t_libusb_call t_libusb_unlock_events)(libusb_context *ctx);
typedef int                           (t_libusb_call t_libusb_event_handling_ok)(libusb_context *ctx);
typedef int                           (t_libusb_call t_libusb_event_handler_active)(libusb_context *ctx);
typedef void                          (t_libusb_call t_libusb_interrupt_event_handler)(libusb_context *ctx);
typedef void                          (t_libusb_call t_libusb_lock_event_waiters)(libusb_context *ctx);
typedef void                          (t_libusb_call t_libusb_unlock_event_waiters)(libusb_context *ctx);
typedef int                           (t_libusb_call t_libusb_wait_for_event)(libusb_context *ctx, struct timeval *tv);

typedef int                           (t_libusb_call t_libusb_handle_events_timeout)(libusb_context *ctx, struct timeval *tv);
typedef int                           (t_libusb_call t_libusb_handle_events_timeout_completed)(libusb_context *ctx, struct timeval *tv, int *completed);
typedef int                           (t_libusb_call t_libusb_handle_events)(libusb_context *ctx);
typedef int                           (t_libusb_call t_libusb_handle_events_completed)(libusb_context *ctx, int *completed);
typedef int                           (t_libusb_call t_libusb_handle_events_locked)(libusb_context *ctx, struct timeval *tv);
typedef int                           (t_libusb_call t_libusb_pollfds_handle_timeouts)(libusb_context *ctx);
typedef int                           (t_libusb_call t_libusb_get_next_timeout)(libusb_context *ctx, struct timeval *tv);

typedef const struct libusb_pollfd ** (t_libusb_call t_libusb_get_pollfds)(libusb_context *ctx);
typedef void                          (t_libusb_call t_libusb_free_pollfds)(const struct libusb_pollfd **pollfds);
typedef void                          (t_libusb_call t_libusb_set_pollfd_notifiers)(libusb_context *ctx, libusb_pollfd_added_cb added_cb, libusb_pollfd_removed_cb removed_cb, void *user_data);

// ****************************************

// typedef void (LIBUSB_CALL *libusb_log_cb)(libusb_context *ctx, enum libusb_log_level level, const char *str);

HMODULE m_libusb_Handle = NULL;

t_libusb_init					  	 			            *p_libusb_init                                     = NULL;

t_libusb_exit                                      *p_libusb_exit                                     = NULL;

t_libusb_set_debug                                 *p_libusb_set_debug                                = NULL;
t_libusb_set_log_cb                                *p_libusb_set_log_cb                               = NULL;
t_libusb_get_version                               *p_libusb_get_version                              = NULL;
t_libusb_has_capability                            *p_libusb_has_capability                           = NULL;
t_libusb_error_name                                *p_libusb_error_name                               = NULL;
t_libusb_setlocale                                 *p_libusb_setlocale                                = NULL;
t_libusb_strerror                                  *p_libusb_strerror                                 = NULL;

t_libusb_get_device_list                           *p_libusb_get_device_list                          = NULL;
t_libusb_free_device_list                          *p_libusb_free_device_list                         = NULL;
t_libusb_ref_device                                *p_libusb_ref_device                               = NULL;
t_libusb_unref_device                              *p_libusb_unref_device                             = NULL;

t_libusb_get_configuration                         *p_libusb_get_configuration                        = NULL;
t_libusb_get_device_descriptor                     *p_libusb_get_device_descriptor                    = NULL;
t_libusb_get_active_config_descriptor              *p_libusb_get_active_config_descriptor             = NULL;
t_libusb_get_config_descriptor                     *p_libusb_get_config_descriptor                    = NULL;
t_libusb_get_config_descriptor_by_value            *p_libusb_get_config_descriptor_by_value           = NULL;
t_libusb_free_config_descriptor                    *p_libusb_free_config_descriptor                   = NULL;
t_libusb_get_ss_endpoint_companion_descriptor      *p_libusb_get_ss_endpoint_companion_descriptor     = NULL;
t_libusb_free_ss_endpoint_companion_descriptor     *p_libusb_free_ss_endpoint_companion_descriptor    = NULL;
t_libusb_get_bos_descriptor                        *p_libusb_get_bos_descriptor                       = NULL;
t_libusb_free_bos_descriptor                       *p_libusb_free_bos_descriptor                      = NULL;
t_libusb_get_usb_2_0_extension_descriptor          *p_libusb_get_usb_2_0_extension_descriptor         = NULL;
t_libusb_free_usb_2_0_extension_descriptor         *p_libusb_free_usb_2_0_extension_descriptor        = NULL;
t_libusb_get_ss_usb_device_capability_descriptor   *p_libusb_get_ss_usb_device_capability_descriptor  = NULL;
t_libusb_free_ss_usb_device_capability_descriptor  *p_libusb_free_ss_usb_device_capability_descriptor = NULL;
t_libusb_get_container_id_descriptor               *p_libusb_get_container_id_descriptor              = NULL;
t_libusb_free_container_id_descriptor              *p_libusb_free_container_id_descriptor             = NULL;
t_libusb_get_bus_number                            *p_libusb_get_bus_number                           = NULL;
t_libusb_get_port_number                           *p_libusb_get_port_number                          = NULL;
t_libusb_get_port_numbers                          *p_libusb_get_port_numbers                         = NULL;

t_libusb_get_port_path                             *p_libusb_get_port_path                            = NULL;
t_libusb_get_parent                                *p_libusb_get_parent                               = NULL;
t_libusb_get_device_address                        *p_libusb_get_device_address                       = NULL;
t_libusb_get_device_speed                          *p_libusb_get_device_speed                         = NULL;
t_libusb_get_max_packet_size                       *p_libusb_get_max_packet_size                      = NULL;
t_libusb_get_max_iso_packet_size                   *p_libusb_get_max_iso_packet_size                  = NULL;

t_libusb_wrap_sys_device                           *p_libusb_wrap_sys_device                          = NULL;
t_libusb_open                                      *p_libusb_open                                     = NULL;
t_libusb_close                                     *p_libusb_close                                    = NULL;
t_libusb_get_device                                *p_libusb_get_device                               = NULL;

t_libusb_set_configuration                         *p_libusb_set_configuration                        = NULL;
t_libusb_claim_interface                           *p_libusb_claim_interface                          = NULL;
t_libusb_release_interface                         *p_libusb_release_interface                        = NULL;

t_libusb_open_device_with_vid_pid                  *p_libusb_open_device_with_vid_pid                 = NULL;

t_libusb_set_interface_alt_setting                 *p_libusb_set_interface_alt_setting                = NULL;
t_libusb_clear_halt                                *p_libusb_clear_halt                               = NULL;
t_libusb_reset_device                              *p_libusb_reset_device                             = NULL;

t_libusb_alloc_streams                             *p_libusb_alloc_streams                            = NULL;
t_libusb_free_streams                              *p_libusb_free_streams                             = NULL;

t_libusb_dev_mem_alloc                             *p_libusb_dev_mem_alloc                            = NULL;
t_libusb_dev_mem_free                              *p_libusb_dev_mem_free                             = NULL;

t_libusb_kernel_driver_active                      *p_libusb_kernel_driver_active                     = NULL;
t_libusb_detach_kernel_driver                      *p_libusb_detach_kernel_driver                     = NULL;
t_libusb_attach_kernel_driver                      *p_libusb_attach_kernel_driver                     = NULL;
t_libusb_set_auto_detach_kernel_driver             *p_libusb_set_auto_detach_kernel_driver            = NULL;

t_libusb_alloc_transfer                            *p_libusb_alloc_transfer                           = NULL;
t_libusb_submit_transfer                           *p_libusb_submit_transfer                          = NULL;
t_libusb_cancel_transfer                           *p_libusb_cancel_transfer                          = NULL;
t_libusb_free_transfer                             *p_libusb_free_transfer                            = NULL;
t_libusb_transfer_set_stream_id                    *p_libusb_transfer_set_stream_id                   = NULL;
t_libusb_transfer_get_stream_id                    *p_libusb_transfer_get_stream_id                   = NULL;

t_libusb_control_transfer                          *p_libusb_control_transfer                         = NULL;
t_libusb_bulk_transfer                             *p_libusb_bulk_transfer                            = NULL;
t_libusb_interrupt_transfer                        *p_libusb_interrupt_transfer                       = NULL;

t_libusb_get_string_descriptor_ascii               *p_libusb_get_string_descriptor_ascii              = NULL;

t_libusb_try_lock_events                           *p_libusb_try_lock_events                          = NULL;
t_libusb_lock_events                               *p_libusb_lock_events                              = NULL;
t_libusb_unlock_events                             *p_libusb_unlock_events                            = NULL;
t_libusb_event_handling_ok                         *p_libusb_event_handling_ok                        = NULL;
t_libusb_event_handler_active                      *p_libusb_event_handler_active                     = NULL;
t_libusb_interrupt_event_handler                   *p_libusb_interrupt_event_handler                  = NULL;
t_libusb_lock_event_waiters                        *p_libusb_lock_event_waiters                       = NULL;
t_libusb_unlock_event_waiters                      *p_libusb_unlock_event_waiters                     = NULL;
t_libusb_wait_for_event                            *p_libusb_wait_for_event                           = NULL;

t_libusb_handle_events_timeout                     *p_libusb_handle_events_timeout                    = NULL;
t_libusb_handle_events_timeout_completed           *p_libusb_handle_events_timeout_completed          = NULL;
t_libusb_handle_events                             *p_libusb_handle_events                            = NULL;
t_libusb_handle_events_completed                   *p_libusb_handle_events_completed                  = NULL;
t_libusb_handle_events_locked                      *p_libusb_handle_events_locked                     = NULL;
t_libusb_pollfds_handle_timeouts                   *p_libusb_pollfds_handle_timeouts                  = NULL;
t_libusb_get_next_timeout                          *p_libusb_get_next_timeout                         = NULL;

t_libusb_get_pollfds                               *p_libusb_get_pollfds                              = NULL;
t_libusb_free_pollfds                              *p_libusb_free_pollfds                             = NULL;
t_libusb_set_pollfd_notifiers                      *p_libusb_set_pollfd_notifiers                     = NULL;

// ****************************************

int __fastcall loadLIBUSB(const char *filename)
{
	if (m_libusb_Handle != NULL)
		return 0;	// already loaded

	if (filename == NULL)
		return -1;

	m_libusb_Handle = LoadLibraryEx(filename, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (m_libusb_Handle == NULL)
		m_libusb_Handle = LoadLibrary(filename);
	if (m_libusb_Handle == NULL)
	{
//		const DWORD error = GetLastError();
//		if (error == ERROR_BAD_EXE_FORMAT)
//		{
//		}
		return -2;
	}

	p_libusb_init                                     = (t_libusb_init                                      *)GetProcAddress(m_libusb_Handle, "libusb_init");
   p_libusb_exit                                     = (t_libusb_exit                                      *)GetProcAddress(m_libusb_Handle, "libusb_exit");

   p_libusb_set_debug                                = (t_libusb_set_debug                                 *)GetProcAddress(m_libusb_Handle, "libusb_set_debug");
   p_libusb_set_log_cb                               = (t_libusb_set_log_cb                                *)GetProcAddress(m_libusb_Handle, "libusb_set_log_cb");
   p_libusb_get_version                              = (t_libusb_get_version                               *)GetProcAddress(m_libusb_Handle, "libusb_get_version");
   p_libusb_has_capability                           = (t_libusb_has_capability                            *)GetProcAddress(m_libusb_Handle, "libusb_has_capability");
   p_libusb_error_name                               = (t_libusb_error_name                                *)GetProcAddress(m_libusb_Handle, "libusb_error_name");
   p_libusb_setlocale                                = (t_libusb_setlocale                                 *)GetProcAddress(m_libusb_Handle, "libusb_setlocale");
   p_libusb_strerror                                 = (t_libusb_strerror                                  *)GetProcAddress(m_libusb_Handle, "libusb_strerror");

   p_libusb_get_device_list                          = (t_libusb_get_device_list                           *)GetProcAddress(m_libusb_Handle, "libusb_get_device_list");
   p_libusb_free_device_list                         = (t_libusb_free_device_list                          *)GetProcAddress(m_libusb_Handle, "libusb_free_device_list");
   p_libusb_ref_device                               = (t_libusb_ref_device                                *)GetProcAddress(m_libusb_Handle, "libusb_ref_device");
   p_libusb_unref_device                             = (t_libusb_unref_device                              *)GetProcAddress(m_libusb_Handle, "libusb_unref_device");

   p_libusb_get_configuration                        = (t_libusb_get_configuration                         *)GetProcAddress(m_libusb_Handle, "libusb_get_configuration");
   p_libusb_get_device_descriptor                    = (t_libusb_get_device_descriptor                     *)GetProcAddress(m_libusb_Handle, "libusb_get_device_descriptor");
   p_libusb_get_active_config_descriptor             = (t_libusb_get_active_config_descriptor              *)GetProcAddress(m_libusb_Handle, "libusb_get_active_config_descriptor");
   p_libusb_get_config_descriptor                    = (t_libusb_get_config_descriptor                     *)GetProcAddress(m_libusb_Handle, "libusb_get_config_descriptor");
   p_libusb_get_config_descriptor_by_value           = (t_libusb_get_config_descriptor_by_value            *)GetProcAddress(m_libusb_Handle, "libusb_get_config_descriptor_by_value");
   p_libusb_free_config_descriptor                   = (t_libusb_free_config_descriptor                    *)GetProcAddress(m_libusb_Handle, "libusb_free_config_descriptor");
   p_libusb_get_ss_endpoint_companion_descriptor     = (t_libusb_get_ss_endpoint_companion_descriptor      *)GetProcAddress(m_libusb_Handle, "libusb_get_ss_endpoint_companion_descriptor");
   p_libusb_free_ss_endpoint_companion_descriptor    = (t_libusb_free_ss_endpoint_companion_descriptor     *)GetProcAddress(m_libusb_Handle, "libusb_free_ss_endpoint_companion_descriptor");
   p_libusb_get_bos_descriptor                       = (t_libusb_get_bos_descriptor                        *)GetProcAddress(m_libusb_Handle, "libusb_get_bos_descriptor");
   p_libusb_free_bos_descriptor                      = (t_libusb_free_bos_descriptor                       *)GetProcAddress(m_libusb_Handle, "libusb_free_bos_descriptor");
   p_libusb_get_usb_2_0_extension_descriptor         = (t_libusb_get_usb_2_0_extension_descriptor          *)GetProcAddress(m_libusb_Handle, "libusb_get_usb_2_0_extension_descriptor");
   p_libusb_free_usb_2_0_extension_descriptor        = (t_libusb_free_usb_2_0_extension_descriptor         *)GetProcAddress(m_libusb_Handle, "libusb_free_usb_2_0_extension_descriptor");
   p_libusb_get_ss_usb_device_capability_descriptor  = (t_libusb_get_ss_usb_device_capability_descriptor   *)GetProcAddress(m_libusb_Handle, "libusb_get_ss_usb_device_capability_descriptor");
   p_libusb_free_ss_usb_device_capability_descriptor = (t_libusb_free_ss_usb_device_capability_descriptor  *)GetProcAddress(m_libusb_Handle, "libusb_free_ss_usb_device_capability_descriptor");
   p_libusb_get_container_id_descriptor              = (t_libusb_get_container_id_descriptor               *)GetProcAddress(m_libusb_Handle, "libusb_get_container_id_descriptor");
   p_libusb_free_container_id_descriptor             = (t_libusb_free_container_id_descriptor              *)GetProcAddress(m_libusb_Handle, "libusb_free_container_id_descriptor");
   p_libusb_get_bus_number                           = (t_libusb_get_bus_number                            *)GetProcAddress(m_libusb_Handle, "libusb_get_bus_number");
   p_libusb_get_port_number                          = (t_libusb_get_port_number                           *)GetProcAddress(m_libusb_Handle, "libusb_get_port_number");
   p_libusb_get_port_numbers                         = (t_libusb_get_port_numbers                          *)GetProcAddress(m_libusb_Handle, "libusb_get_port_numbers");

   p_libusb_get_port_path                            = (t_libusb_get_port_path                             *)GetProcAddress(m_libusb_Handle, "libusb_get_port_path");
   p_libusb_get_parent                               = (t_libusb_get_parent                                *)GetProcAddress(m_libusb_Handle, "libusb_get_parent");
   p_libusb_get_device_address                       = (t_libusb_get_device_address                        *)GetProcAddress(m_libusb_Handle, "libusb_get_device_address");
   p_libusb_get_device_speed                         = (t_libusb_get_device_speed                          *)GetProcAddress(m_libusb_Handle, "libusb_get_device_speed");
   p_libusb_get_max_packet_size                      = (t_libusb_get_max_packet_size                       *)GetProcAddress(m_libusb_Handle, "libusb_get_max_packet_size");
   p_libusb_get_max_iso_packet_size                  = (t_libusb_get_max_iso_packet_size                   *)GetProcAddress(m_libusb_Handle, "libusb_get_max_iso_packet_size");

   p_libusb_wrap_sys_device                          = (t_libusb_wrap_sys_device                           *)GetProcAddress(m_libusb_Handle, "libusb_wrap_sys_device");
   p_libusb_open                                     = (t_libusb_open                                      *)GetProcAddress(m_libusb_Handle, "libusb_open");
   p_libusb_close                                    = (t_libusb_close                                     *)GetProcAddress(m_libusb_Handle, "libusb_close");
   p_libusb_get_device                               = (t_libusb_get_device                                *)GetProcAddress(m_libusb_Handle, "libusb_get_device");

   p_libusb_set_configuration                        = (t_libusb_set_configuration                         *)GetProcAddress(m_libusb_Handle, "libusb_set_configuration");
   p_libusb_claim_interface                          = (t_libusb_claim_interface                           *)GetProcAddress(m_libusb_Handle, "libusb_claim_interface");
   p_libusb_release_interface                        = (t_libusb_release_interface                         *)GetProcAddress(m_libusb_Handle, "libusb_release_interface");

   p_libusb_open_device_with_vid_pid                 = (t_libusb_open_device_with_vid_pid                  *)GetProcAddress(m_libusb_Handle, "libusb_open_device_with_vid_pid");

   p_libusb_set_interface_alt_setting                = (t_libusb_set_interface_alt_setting                 *)GetProcAddress(m_libusb_Handle, "libusb_set_interface_alt_setting");
   p_libusb_clear_halt                               = (t_libusb_clear_halt                                *)GetProcAddress(m_libusb_Handle, "libusb_clear_halt");
   p_libusb_reset_device                             = (t_libusb_reset_device                              *)GetProcAddress(m_libusb_Handle, "libusb_reset_device");

   p_libusb_alloc_streams                            = (t_libusb_alloc_streams                             *)GetProcAddress(m_libusb_Handle, "libusb_alloc_streams");
   p_libusb_free_streams                             = (t_libusb_free_streams                              *)GetProcAddress(m_libusb_Handle, "libusb_free_streams");

   p_libusb_dev_mem_alloc                            = (t_libusb_dev_mem_alloc                             *)GetProcAddress(m_libusb_Handle, "libusb_dev_mem_alloc");
   p_libusb_dev_mem_free                             = (t_libusb_dev_mem_free                              *)GetProcAddress(m_libusb_Handle, "libusb_dev_mem_free");

   p_libusb_kernel_driver_active                     = (t_libusb_kernel_driver_active                      *)GetProcAddress(m_libusb_Handle, "libusb_kernel_driver_active");
   p_libusb_detach_kernel_driver                     = (t_libusb_detach_kernel_driver                      *)GetProcAddress(m_libusb_Handle, "libusb_detach_kernel_driver");
   p_libusb_attach_kernel_driver                     = (t_libusb_attach_kernel_driver                      *)GetProcAddress(m_libusb_Handle, "libusb_attach_kernel_driver");
   p_libusb_set_auto_detach_kernel_driver            = (t_libusb_set_auto_detach_kernel_driver             *)GetProcAddress(m_libusb_Handle, "libusb_set_auto_detach_kernel_driver");

   p_libusb_alloc_transfer                           = (t_libusb_alloc_transfer                            *)GetProcAddress(m_libusb_Handle, "libusb_alloc_transfer");
   p_libusb_submit_transfer                          = (t_libusb_submit_transfer                           *)GetProcAddress(m_libusb_Handle, "libusb_submit_transfer");
   p_libusb_cancel_transfer                          = (t_libusb_cancel_transfer                           *)GetProcAddress(m_libusb_Handle, "libusb_cancel_transfer");
   p_libusb_free_transfer                            = (t_libusb_free_transfer                             *)GetProcAddress(m_libusb_Handle, "libusb_free_transfer");
   p_libusb_transfer_set_stream_id                   = (t_libusb_transfer_set_stream_id                    *)GetProcAddress(m_libusb_Handle, "libusb_transfer_set_stream_id");
   p_libusb_transfer_get_stream_id                   = (t_libusb_transfer_get_stream_id                    *)GetProcAddress(m_libusb_Handle, "libusb_transfer_get_stream_id");

   p_libusb_control_transfer                         = (t_libusb_control_transfer                          *)GetProcAddress(m_libusb_Handle, "libusb_control_transfer");
   p_libusb_bulk_transfer                            = (t_libusb_bulk_transfer                             *)GetProcAddress(m_libusb_Handle, "libusb_bulk_transfer");
   p_libusb_interrupt_transfer                       = (t_libusb_interrupt_transfer                        *)GetProcAddress(m_libusb_Handle, "libusb_interrupt_transfer");

   p_libusb_get_string_descriptor_ascii              = (t_libusb_get_string_descriptor_ascii               *)GetProcAddress(m_libusb_Handle, "libusb_get_string_descriptor_ascii");

   p_libusb_try_lock_events                          = (t_libusb_try_lock_events                           *)GetProcAddress(m_libusb_Handle, "libusb_try_lock_events");
   p_libusb_lock_events                              = (t_libusb_lock_events                               *)GetProcAddress(m_libusb_Handle, "libusb_lock_events");
   p_libusb_unlock_events                            = (t_libusb_unlock_events                             *)GetProcAddress(m_libusb_Handle, "libusb_unlock_events");
   p_libusb_event_handling_ok                        = (t_libusb_event_handling_ok                         *)GetProcAddress(m_libusb_Handle, "libusb_event_handling_ok");
   p_libusb_event_handler_active                     = (t_libusb_event_handler_active                      *)GetProcAddress(m_libusb_Handle, "libusb_event_handler_active");
   p_libusb_interrupt_event_handler                  = (t_libusb_interrupt_event_handler                   *)GetProcAddress(m_libusb_Handle, "libusb_interrupt_event_handler");
   p_libusb_lock_event_waiters                       = (t_libusb_lock_event_waiters                        *)GetProcAddress(m_libusb_Handle, "libusb_lock_event_waiters");
   p_libusb_unlock_event_waiters                     = (t_libusb_unlock_event_waiters                      *)GetProcAddress(m_libusb_Handle, "libusb_unlock_event_waiters");
   p_libusb_wait_for_event                           = (t_libusb_wait_for_event                            *)GetProcAddress(m_libusb_Handle, "libusb_wait_for_event");

   p_libusb_handle_events_timeout                    = (t_libusb_handle_events_timeout                     *)GetProcAddress(m_libusb_Handle, "libusb_handle_events_timeout");
   p_libusb_handle_events_timeout_completed          = (t_libusb_handle_events_timeout_completed           *)GetProcAddress(m_libusb_Handle, "libusb_handle_events_timeout_completed");
   p_libusb_handle_events                            = (t_libusb_handle_events                             *)GetProcAddress(m_libusb_Handle, "libusb_handle_events");
   p_libusb_handle_events_completed                  = (t_libusb_handle_events_completed                   *)GetProcAddress(m_libusb_Handle, "libusb_handle_events_completed");
   p_libusb_handle_events_locked                     = (t_libusb_handle_events_locked                      *)GetProcAddress(m_libusb_Handle, "libusb_handle_events_locked");
   p_libusb_pollfds_handle_timeouts                  = (t_libusb_pollfds_handle_timeouts                   *)GetProcAddress(m_libusb_Handle, "libusb_pollfds_handle_timeouts");
   p_libusb_get_next_timeout                         = (t_libusb_get_next_timeout                          *)GetProcAddress(m_libusb_Handle, "libusb_get_next_timeout");

   p_libusb_get_pollfds                              = (t_libusb_get_pollfds                               *)GetProcAddress(m_libusb_Handle, "libusb_get_pollfds");
   p_libusb_free_pollfds                             = (t_libusb_free_pollfds                              *)GetProcAddress(m_libusb_Handle, "libusb_free_pollfds");
   p_libusb_set_pollfd_notifiers                     = (t_libusb_set_pollfd_notifiers                      *)GetProcAddress(m_libusb_Handle, "libusb_set_pollfd_notifiers");


   int errors = 0;

	if (p_libusb_init                                     == NULL) errors++;
   if (p_libusb_exit                                     == NULL) errors++;

   if (p_libusb_set_debug                                == NULL) errors++;
   if (p_libusb_set_log_cb                               == NULL) errors++;
   if (p_libusb_get_version                              == NULL) errors++;
   if (p_libusb_has_capability                           == NULL) errors++;
   if (p_libusb_error_name                               == NULL) errors++;
   if (p_libusb_setlocale                                == NULL) errors++;
   if (p_libusb_strerror                                 == NULL) errors++;

   if (p_libusb_get_device_list                          == NULL) errors++;
   if (p_libusb_free_device_list                         == NULL) errors++;
   if (p_libusb_ref_device                               == NULL) errors++;
   if (p_libusb_unref_device                             == NULL) errors++;

   if (p_libusb_get_configuration                        == NULL) errors++;
   if (p_libusb_get_device_descriptor                    == NULL) errors++;
   if (p_libusb_get_active_config_descriptor             == NULL) errors++;
   if (p_libusb_get_config_descriptor                    == NULL) errors++;
   if (p_libusb_get_config_descriptor_by_value           == NULL) errors++;
   if (p_libusb_free_config_descriptor                   == NULL) errors++;
   if (p_libusb_get_ss_endpoint_companion_descriptor     == NULL) errors++;
   if (p_libusb_free_ss_endpoint_companion_descriptor    == NULL) errors++;
   if (p_libusb_get_bos_descriptor                       == NULL) errors++;
   if (p_libusb_free_bos_descriptor                      == NULL) errors++;
   if (p_libusb_get_usb_2_0_extension_descriptor         == NULL) errors++;
   if (p_libusb_free_usb_2_0_extension_descriptor        == NULL) errors++;
   if (p_libusb_get_ss_usb_device_capability_descriptor  == NULL) errors++;
   if (p_libusb_free_ss_usb_device_capability_descriptor == NULL) errors++;
   if (p_libusb_get_container_id_descriptor              == NULL) errors++;
   if (p_libusb_free_container_id_descriptor             == NULL) errors++;
   if (p_libusb_get_bus_number                           == NULL) errors++;
   if (p_libusb_get_port_number                          == NULL) errors++;
   if (p_libusb_get_port_numbers                         == NULL) errors++;

   if (p_libusb_get_port_path                            == NULL) errors++;
   if (p_libusb_get_parent                               == NULL) errors++;
   if (p_libusb_get_device_address                       == NULL) errors++;
   if (p_libusb_get_device_speed                         == NULL) errors++;
   if (p_libusb_get_max_packet_size                      == NULL) errors++;
   if (p_libusb_get_max_iso_packet_size                  == NULL) errors++;

// if (p_libusb_wrap_sys_device                          == NULL) errors++;
   if (p_libusb_open                                     == NULL) errors++;
   if (p_libusb_close                                    == NULL) errors++;
   if (p_libusb_get_device                               == NULL) errors++;

   if (p_libusb_set_configuration                        == NULL) errors++;
   if (p_libusb_claim_interface                          == NULL) errors++;
   if (p_libusb_release_interface                        == NULL) errors++;

   if (p_libusb_open_device_with_vid_pid                 == NULL) errors++;

   if (p_libusb_set_interface_alt_setting                == NULL) errors++;
   if (p_libusb_clear_halt                               == NULL) errors++;
   if (p_libusb_reset_device                             == NULL) errors++;

   if (p_libusb_alloc_streams                            == NULL) errors++;
   if (p_libusb_free_streams                             == NULL) errors++;

   if (p_libusb_dev_mem_alloc                            == NULL) errors++;
   if (p_libusb_dev_mem_free                             == NULL) errors++;
   if (p_libusb_kernel_driver_active                     == NULL) errors++;

   if (p_libusb_detach_kernel_driver                     == NULL) errors++;
   if (p_libusb_attach_kernel_driver                     == NULL) errors++;
   if (p_libusb_set_auto_detach_kernel_driver            == NULL) errors++;

   if (p_libusb_alloc_transfer                           == NULL) errors++;
   if (p_libusb_submit_transfer                          == NULL) errors++;
   if (p_libusb_cancel_transfer                          == NULL) errors++;
   if (p_libusb_free_transfer                            == NULL) errors++;
   if (p_libusb_transfer_set_stream_id                   == NULL) errors++;
   if (p_libusb_transfer_get_stream_id                   == NULL) errors++;

   if (p_libusb_control_transfer                         == NULL) errors++;
   if (p_libusb_bulk_transfer                            == NULL) errors++;
   if (p_libusb_interrupt_transfer                       == NULL) errors++;

   if (p_libusb_get_string_descriptor_ascii              == NULL) errors++;

   if (p_libusb_try_lock_events                          == NULL) errors++;
   if (p_libusb_lock_events                              == NULL) errors++;
   if (p_libusb_unlock_events                            == NULL) errors++;
   if (p_libusb_event_handling_ok                        == NULL) errors++;
   if (p_libusb_event_handler_active                     == NULL) errors++;
   if (p_libusb_interrupt_event_handler                  == NULL) errors++;
   if (p_libusb_lock_event_waiters                       == NULL) errors++;
   if (p_libusb_unlock_event_waiters                     == NULL) errors++;
   if (p_libusb_wait_for_event                           == NULL) errors++;

   if (p_libusb_handle_events_timeout                    == NULL) errors++;
   if (p_libusb_handle_events_timeout_completed          == NULL) errors++;
   if (p_libusb_handle_events                            == NULL) errors++;
   if (p_libusb_handle_events_completed                  == NULL) errors++;
   if (p_libusb_handle_events_locked                     == NULL) errors++;
   if (p_libusb_pollfds_handle_timeouts                  == NULL) errors++;
   if (p_libusb_get_next_timeout                         == NULL) errors++;

   if (p_libusb_get_pollfds                              == NULL) errors++;
   if (p_libusb_free_pollfds                             == NULL) errors++;
   if (p_libusb_set_pollfd_notifiers                     == NULL) errors++;

   if (errors > 0)
	{
		unloadLIBUSB();
		return -2;
	}

	return 0;
}

void __fastcall unloadLIBUSB()
{
	p_libusb_init                                     = NULL;
   p_libusb_exit                                     = NULL;

   p_libusb_set_debug                                = NULL;
   p_libusb_set_log_cb                               = NULL;
   p_libusb_get_version                              = NULL;
   p_libusb_has_capability                           = NULL;
   p_libusb_error_name                               = NULL;
   p_libusb_setlocale                                = NULL;
   p_libusb_strerror                                 = NULL;

   p_libusb_get_device_list                          = NULL;
   p_libusb_free_device_list                         = NULL;
   p_libusb_ref_device                               = NULL;
   p_libusb_unref_device                             = NULL;

   p_libusb_get_configuration                        = NULL;
   p_libusb_get_device_descriptor                    = NULL;
   p_libusb_get_active_config_descriptor             = NULL;
   p_libusb_get_config_descriptor                    = NULL;
   p_libusb_get_config_descriptor_by_value           = NULL;
   p_libusb_free_config_descriptor                   = NULL;
   p_libusb_get_ss_endpoint_companion_descriptor     = NULL;
   p_libusb_free_ss_endpoint_companion_descriptor    = NULL;
   p_libusb_get_bos_descriptor                       = NULL;
   p_libusb_free_bos_descriptor                      = NULL;
   p_libusb_get_usb_2_0_extension_descriptor         = NULL;
   p_libusb_free_usb_2_0_extension_descriptor        = NULL;
   p_libusb_get_ss_usb_device_capability_descriptor  = NULL;
   p_libusb_free_ss_usb_device_capability_descriptor = NULL;
   p_libusb_get_container_id_descriptor              = NULL;
   p_libusb_free_container_id_descriptor             = NULL;
   p_libusb_get_bus_number                           = NULL;
   p_libusb_get_port_number                          = NULL;
   p_libusb_get_port_numbers                         = NULL;

   p_libusb_get_port_path                            = NULL;
   p_libusb_get_parent                               = NULL;
   p_libusb_get_device_address                       = NULL;
   p_libusb_get_device_speed                         = NULL;
   p_libusb_get_max_packet_size                      = NULL;
   p_libusb_get_max_iso_packet_size                  = NULL;

   p_libusb_wrap_sys_device                          = NULL;
   p_libusb_open                                     = NULL;
   p_libusb_close                                    = NULL;
   p_libusb_get_device                               = NULL;

   p_libusb_set_configuration                        = NULL;
   p_libusb_claim_interface                          = NULL;
   p_libusb_release_interface                        = NULL;

   p_libusb_open_device_with_vid_pid                 = NULL;

   p_libusb_set_interface_alt_setting                = NULL;
   p_libusb_clear_halt                               = NULL;
   p_libusb_reset_device                             = NULL;

   p_libusb_alloc_streams                            = NULL;
   p_libusb_free_streams                             = NULL;

   p_libusb_dev_mem_alloc                            = NULL;
   p_libusb_dev_mem_free                             = NULL;
   p_libusb_kernel_driver_active                     = NULL;

   p_libusb_detach_kernel_driver                     = NULL;
   p_libusb_attach_kernel_driver                     = NULL;
   p_libusb_set_auto_detach_kernel_driver            = NULL;

   p_libusb_alloc_transfer                           = NULL;
   p_libusb_submit_transfer                          = NULL;
   p_libusb_cancel_transfer                          = NULL;
   p_libusb_free_transfer                            = NULL;
   p_libusb_transfer_set_stream_id                   = NULL;
   p_libusb_transfer_get_stream_id                   = NULL;

   p_libusb_control_transfer                         = NULL;
   p_libusb_bulk_transfer                            = NULL;
   p_libusb_interrupt_transfer                       = NULL;

   p_libusb_get_string_descriptor_ascii              = NULL;

   p_libusb_try_lock_events                          = NULL;
   p_libusb_lock_events                              = NULL;
   p_libusb_unlock_events                            = NULL;
   p_libusb_event_handling_ok                        = NULL;
   p_libusb_event_handler_active                     = NULL;
   p_libusb_interrupt_event_handler                  = NULL;
   p_libusb_lock_event_waiters                       = NULL;
   p_libusb_unlock_event_waiters                     = NULL;
   p_libusb_wait_for_event                           = NULL;

   p_libusb_handle_events_timeout                    = NULL;
   p_libusb_handle_events_timeout_completed          = NULL;
   p_libusb_handle_events                            = NULL;
   p_libusb_handle_events_completed                  = NULL;
   p_libusb_handle_events_locked                     = NULL;
   p_libusb_pollfds_handle_timeouts                  = NULL;
   p_libusb_get_next_timeout                         = NULL;

   p_libusb_get_pollfds                              = NULL;
   p_libusb_free_pollfds                             = NULL;
   p_libusb_set_pollfd_notifiers                     = NULL;

	if (m_libusb_Handle != NULL)
		FreeLibrary(m_libusb_Handle);
	m_libusb_Handle = NULL;
}

//int LIBUSB_CALL libusb_init(libusb_context **ctx);
int libusb_init(libusb_context **ctx)
{
	return (p_libusb_init) ? p_libusb_init(ctx) : LIBUSB_ERROR_OTHER;
}

void libusb_exit(libusb_context *ctx)
{
   if (p_libusb_exit)
      p_libusb_exit(ctx);
}

void libusb_set_debug(libusb_context *ctx, int level)
{
   if (p_libusb_set_debug)
      p_libusb_set_debug(ctx, level);
}

void libusb_set_log_cb(libusb_context *ctx, libusb_log_cb cb, int mode)
{
   if (p_libusb_set_log_cb)
      p_libusb_set_log_cb(ctx, cb, mode);
}

const struct libusb_version * libusb_get_version(void)
{
   return (p_libusb_get_version) ? p_libusb_get_version() : NULL;
}

int libusb_has_capability(uint32_t capability)
{
   return (p_libusb_has_capability) ? p_libusb_has_capability(capability) : LIBUSB_ERROR_OTHER;
}

const char * libusb_error_name(int errcode)
{
   return (p_libusb_error_name) ? p_libusb_error_name(errcode) : NULL;
}

int libusb_setlocale(const char *locale)
{
   return (p_libusb_setlocale) ? p_libusb_setlocale(locale) : LIBUSB_ERROR_OTHER;
}

const char * libusb_strerror(int errcode)
{
   return (p_libusb_strerror) ? p_libusb_strerror(errcode) : NULL;
}

ssize_t libusb_get_device_list(libusb_context *ctx, libusb_device ***list)
{
   return (p_libusb_get_device_list) ? p_libusb_get_device_list(ctx, list) : 0;
}

void libusb_free_device_list(libusb_device **list, int unref_devices)
{
   if (p_libusb_free_device_list)
      p_libusb_free_device_list(list, unref_devices);
}

libusb_device * libusb_ref_device(libusb_device *dev)
{
   return (p_libusb_ref_device) ? p_libusb_ref_device(dev) : NULL;
}

void libusb_unref_device(libusb_device *dev)
{
   if (p_libusb_unref_device)
      p_libusb_unref_device(dev);
}

int libusb_get_configuration(libusb_device_handle *dev, int *config)
{
   return (p_libusb_get_configuration) ? p_libusb_get_configuration(dev, config) : LIBUSB_ERROR_OTHER;
}

int libusb_get_device_descriptor(libusb_device *dev, struct libusb_device_descriptor *desc)
{
   return (p_libusb_get_device_descriptor) ? p_libusb_get_device_descriptor(dev, desc) : LIBUSB_ERROR_OTHER;
}

int libusb_get_active_config_descriptor(libusb_device *dev, struct libusb_config_descriptor **config)
{
   return (p_libusb_get_active_config_descriptor) ? p_libusb_get_active_config_descriptor(dev, config) : LIBUSB_ERROR_OTHER;
}

int libusb_get_config_descriptor(libusb_device *dev, uint8_t config_index, struct libusb_config_descriptor **config)
{
   return (p_libusb_get_config_descriptor) ? p_libusb_get_config_descriptor(dev, config_index, config) : LIBUSB_ERROR_OTHER;
}

int libusb_get_config_descriptor_by_value(libusb_device *dev, uint8_t bConfigurationValue, struct libusb_config_descriptor **config)
{
   return (p_libusb_get_config_descriptor_by_value) ? p_libusb_get_config_descriptor_by_value(dev, bConfigurationValue, config) : LIBUSB_ERROR_OTHER;
}

void libusb_free_config_descriptor(struct libusb_config_descriptor *config)
{
   if (p_libusb_free_config_descriptor)
      p_libusb_free_config_descriptor(config);
}

int libusb_get_ss_endpoint_companion_descriptor(struct libusb_context *ctx, const struct libusb_endpoint_descriptor *endpoint, struct libusb_ss_endpoint_companion_descriptor **ep_comp)
{
   return (p_libusb_get_ss_endpoint_companion_descriptor) ? p_libusb_get_ss_endpoint_companion_descriptor(ctx, endpoint, ep_comp) : LIBUSB_ERROR_OTHER;
}

void libusb_free_ss_endpoint_companion_descriptor(struct libusb_ss_endpoint_companion_descriptor *ep_comp)
{
   if (p_libusb_free_ss_endpoint_companion_descriptor)
      p_libusb_free_ss_endpoint_companion_descriptor(ep_comp);
}

int libusb_get_bos_descriptor(libusb_device_handle *dev_handle, struct libusb_bos_descriptor **bos)
{
   return (p_libusb_get_bos_descriptor) ? p_libusb_get_bos_descriptor(dev_handle, bos) : LIBUSB_ERROR_OTHER;
}

void libusb_free_bos_descriptor(struct libusb_bos_descriptor *bos)
{
   if (p_libusb_free_bos_descriptor)
      p_libusb_free_bos_descriptor(bos);
}

int libusb_get_usb_2_0_extension_descriptor(struct libusb_context *ctx, struct libusb_bos_dev_capability_descriptor *dev_cap, struct libusb_usb_2_0_extension_descriptor **usb_2_0_extension)
{
   return (p_libusb_get_usb_2_0_extension_descriptor) ? p_libusb_get_usb_2_0_extension_descriptor(ctx, dev_cap, usb_2_0_extension) : LIBUSB_ERROR_OTHER;
}

void libusb_free_usb_2_0_extension_descriptor(struct libusb_usb_2_0_extension_descriptor *usb_2_0_extension)
{
   if (p_libusb_free_usb_2_0_extension_descriptor)
      p_libusb_free_usb_2_0_extension_descriptor(usb_2_0_extension);
}

int libusb_get_ss_usb_device_capability_descriptor(struct libusb_context *ctx, struct libusb_bos_dev_capability_descriptor *dev_cap, struct libusb_ss_usb_device_capability_descriptor **ss_usb_device_cap)
{
   return (p_libusb_get_ss_usb_device_capability_descriptor) ? p_libusb_get_ss_usb_device_capability_descriptor(ctx, dev_cap, ss_usb_device_cap) : LIBUSB_ERROR_OTHER;
}

void libusb_free_ss_usb_device_capability_descriptor(struct libusb_ss_usb_device_capability_descriptor *ss_usb_device_cap)
{
   if (p_libusb_free_ss_usb_device_capability_descriptor)
      p_libusb_free_ss_usb_device_capability_descriptor(ss_usb_device_cap);
}

int libusb_get_container_id_descriptor(struct libusb_context *ctx, struct libusb_bos_dev_capability_descriptor *dev_cap, struct libusb_container_id_descriptor **container_id)
{
   return (p_libusb_get_container_id_descriptor) ? p_libusb_get_container_id_descriptor(ctx, dev_cap, container_id) : LIBUSB_ERROR_OTHER;
}

void libusb_free_container_id_descriptor(struct libusb_container_id_descriptor *container_id)
{
   if (p_libusb_free_container_id_descriptor)
      p_libusb_free_container_id_descriptor(container_id);
}

uint8_t libusb_get_bus_number(libusb_device *dev)
{
   return (p_libusb_get_bus_number) ? p_libusb_get_bus_number(dev) : LIBUSB_ERROR_OTHER;
}

uint8_t libusb_get_port_number(libusb_device *dev)
{
   return (p_libusb_get_port_number) ? p_libusb_get_port_number(dev) : 0;
}

int libusb_get_port_numbers(libusb_device *dev, uint8_t *port_numbers, int port_numbers_len)
{
   return (p_libusb_get_port_numbers) ? p_libusb_get_port_numbers(dev, port_numbers, port_numbers_len) : LIBUSB_ERROR_OTHER;
}

int libusb_get_port_path(libusb_context *ctx, libusb_device *dev, uint8_t *path, uint8_t path_length)
{
   return (p_libusb_get_port_path) ? p_libusb_get_port_path(ctx, dev, path, path_length) : LIBUSB_ERROR_OTHER;
}

libusb_device * libusb_get_parent(libusb_device *dev)
{
   return (p_libusb_get_parent) ? p_libusb_get_parent(dev) : NULL;
}

uint8_t libusb_get_device_address(libusb_device *dev)
{
   return (p_libusb_get_device_address) ? p_libusb_get_device_address(dev) : 0;
}

int libusb_get_device_speed(libusb_device *dev)
{
   return (p_libusb_get_device_speed) ? p_libusb_get_device_speed(dev) : LIBUSB_ERROR_OTHER;
}

int libusb_get_max_packet_size(libusb_device *dev, unsigned char endpoint)
{
   return (p_libusb_get_max_packet_size) ? p_libusb_get_max_packet_size(dev, endpoint) : LIBUSB_ERROR_OTHER;
}

int libusb_get_max_iso_packet_size(libusb_device *dev, unsigned char endpoint)
{
   return (p_libusb_get_max_iso_packet_size) ? p_libusb_get_max_iso_packet_size(dev, endpoint) : LIBUSB_ERROR_OTHER;
}

// this is not present in the DLL
int libusb_wrap_sys_device(libusb_context *ctx, intptr_t sys_dev, libusb_device_handle **dev_handle)
{
   return (p_libusb_wrap_sys_device) ? p_libusb_wrap_sys_device(ctx, sys_dev, dev_handle) : LIBUSB_ERROR_OTHER;
}

int libusb_open(libusb_device *dev, libusb_device_handle **dev_handle)
{
   return (p_libusb_open) ? p_libusb_open(dev, dev_handle) : LIBUSB_ERROR_OTHER;
}

void libusb_close(libusb_device_handle *dev_handle)
{
   if (p_libusb_close)
      p_libusb_close(dev_handle);
}

libusb_device * libusb_get_device(libusb_device_handle *dev_handle)
{
   return (p_libusb_get_device) ? p_libusb_get_device(dev_handle) : NULL;
}

int libusb_set_configuration(libusb_device_handle *dev_handle, int configuration)
{
   return (p_libusb_set_configuration) ? p_libusb_set_configuration(dev_handle, configuration) : LIBUSB_ERROR_OTHER;
}

int libusb_claim_interface(libusb_device_handle *dev_handle, int interface_number)
{
   return (p_libusb_claim_interface) ? p_libusb_claim_interface(dev_handle, interface_number) : LIBUSB_ERROR_OTHER;
}

int libusb_release_interface(libusb_device_handle *dev_handle, int interface_number)
{
   return (p_libusb_release_interface) ? p_libusb_release_interface(dev_handle, interface_number) : LIBUSB_ERROR_OTHER;
}

libusb_device_handle * libusb_open_device_with_vid_pid(libusb_context *ctx, uint16_t vendor_id, uint16_t product_id)
{
   return (p_libusb_open_device_with_vid_pid) ? p_libusb_open_device_with_vid_pid(ctx, vendor_id, product_id) : NULL;
}

int libusb_set_interface_alt_setting(libusb_device_handle *dev_handle, int interface_number, int alternate_setting)
{
   return (p_libusb_set_interface_alt_setting) ? p_libusb_set_interface_alt_setting(dev_handle, interface_number, alternate_setting) : LIBUSB_ERROR_OTHER;
}

int libusb_clear_halt(libusb_device_handle *dev_handle, unsigned char endpoint)
{
   return (p_libusb_clear_halt) ? p_libusb_clear_halt(dev_handle, endpoint) : LIBUSB_ERROR_OTHER;
}

int libusb_reset_device(libusb_device_handle *dev_handle)
{
   return (p_libusb_reset_device) ? p_libusb_reset_device(dev_handle) : LIBUSB_ERROR_OTHER;
}

int libusb_alloc_streams(libusb_device_handle *dev_handle, uint32_t num_streams, unsigned char *endpoints, int num_endpoints)
{
   return (p_libusb_alloc_streams) ? p_libusb_alloc_streams(dev_handle, num_streams, endpoints, num_endpoints) : LIBUSB_ERROR_OTHER;
}

int libusb_free_streams(libusb_device_handle *dev_handle, unsigned char *endpoints, int num_endpoints)
{
   return (p_libusb_free_streams) ? p_libusb_free_streams(dev_handle, endpoints, num_endpoints) : LIBUSB_ERROR_OTHER;
}

unsigned char * libusb_dev_mem_alloc(libusb_device_handle *dev_handle, size_t length)
{
   return (p_libusb_dev_mem_alloc) ? p_libusb_dev_mem_alloc(dev_handle, length) : NULL;
}

int libusb_dev_mem_free(libusb_device_handle *dev_handle, unsigned char *buffer, size_t length)
{
   return (p_libusb_dev_mem_free) ? p_libusb_dev_mem_free(dev_handle, buffer, length) : LIBUSB_ERROR_OTHER;
}

int libusb_kernel_driver_active(libusb_device_handle *dev_handle, int interface_number)
{
	return (p_libusb_kernel_driver_active) ? p_libusb_kernel_driver_active(dev_handle, interface_number) : LIBUSB_ERROR_OTHER;
}

int libusb_detach_kernel_driver(libusb_device_handle *dev_handle, int interface_number)
{
	return (p_libusb_detach_kernel_driver) ? p_libusb_detach_kernel_driver(dev_handle, interface_number) : LIBUSB_ERROR_OTHER;
}

int libusb_attach_kernel_driver(libusb_device_handle *dev_handle, int interface_number)
{
	return (p_libusb_attach_kernel_driver) ? p_libusb_attach_kernel_driver(dev_handle, interface_number) : LIBUSB_ERROR_OTHER;
}

int libusb_set_auto_detach_kernel_driver(libusb_device_handle *dev_handle, int enable)
{
	return (p_libusb_set_auto_detach_kernel_driver) ? p_libusb_set_auto_detach_kernel_driver(dev_handle, enable) : LIBUSB_ERROR_OTHER;
}

struct libusb_transfer * libusb_alloc_transfer(int iso_packets)
{
	return (p_libusb_alloc_transfer) ? p_libusb_alloc_transfer(iso_packets) : NULL;
}

int libusb_submit_transfer(struct libusb_transfer *transfer)
{
	return (p_libusb_submit_transfer) ? p_libusb_submit_transfer(transfer) : LIBUSB_ERROR_OTHER;
}

int libusb_cancel_transfer(struct libusb_transfer *transfer)
{
	return (p_libusb_cancel_transfer) ? p_libusb_cancel_transfer(transfer) : LIBUSB_ERROR_OTHER;
}

void libusb_free_transfer(struct libusb_transfer *transfer)
{
	if (p_libusb_free_transfer)
      p_libusb_free_transfer(transfer);
}

void libusb_transfer_set_stream_id(struct libusb_transfer *transfer, uint32_t stream_id)
{
	if (p_libusb_transfer_set_stream_id)
      p_libusb_transfer_set_stream_id(transfer, stream_id);
}

uint32_t libusb_transfer_get_stream_id(struct libusb_transfer *transfer)
{
	return (p_libusb_transfer_get_stream_id) ? p_libusb_transfer_get_stream_id(transfer) : LIBUSB_ERROR_OTHER;
}

int libusb_control_transfer(libusb_device_handle *dev_handle, uint8_t request_type, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, unsigned char *data, uint16_t wLength, unsigned int timeout)
{
	return (p_libusb_control_transfer) ? p_libusb_control_transfer(dev_handle, request_type, bRequest, wValue, wIndex, data, wLength, timeout) : LIBUSB_ERROR_OTHER;
}

int libusb_bulk_transfer(libusb_device_handle *dev_handle, unsigned char endpoint, unsigned char *data, int length, int *actual_length, unsigned int timeout)
{
	return (p_libusb_bulk_transfer) ? p_libusb_bulk_transfer(dev_handle, endpoint, data, length, actual_length, timeout) : LIBUSB_ERROR_OTHER;
}

int libusb_interrupt_transfer(libusb_device_handle *dev_handle, unsigned char endpoint, unsigned char *data, int length, int *actual_length, unsigned int timeout)
{
	return (p_libusb_interrupt_transfer) ? p_libusb_interrupt_transfer(dev_handle, endpoint, data, length, actual_length, timeout) : LIBUSB_ERROR_OTHER;
}

int libusb_get_string_descriptor_ascii(libusb_device_handle *dev_handle, uint8_t desc_index, unsigned char *data, int length)
{
	return (p_libusb_get_string_descriptor_ascii) ? p_libusb_get_string_descriptor_ascii(dev_handle, desc_index, data, length) : LIBUSB_ERROR_OTHER;
}

int libusb_try_lock_events(libusb_context *ctx)
{
	return (p_libusb_try_lock_events) ? p_libusb_try_lock_events(ctx) : LIBUSB_ERROR_OTHER;
}

void libusb_lock_events(libusb_context *ctx)
{
	if (p_libusb_lock_events)
      p_libusb_lock_events(ctx);
}

void libusb_unlock_events(libusb_context *ctx)
{
	if (p_libusb_unlock_events)
      p_libusb_unlock_events(ctx);
}

int libusb_event_handling_ok(libusb_context *ctx)
{
	return (p_libusb_event_handling_ok) ? p_libusb_event_handling_ok(ctx) : LIBUSB_ERROR_OTHER;
}

int libusb_event_handler_active(libusb_context *ctx)
{
	return (p_libusb_event_handler_active) ? p_libusb_event_handler_active(ctx) : LIBUSB_ERROR_OTHER;
}

void libusb_interrupt_event_handler(libusb_context *ctx)
{
	if (p_libusb_interrupt_event_handler)
      p_libusb_interrupt_event_handler(ctx);
}

void libusb_lock_event_waiters(libusb_context *ctx)
{
	if (p_libusb_lock_event_waiters)
      p_libusb_lock_event_waiters(ctx);
}

void libusb_unlock_event_waiters(libusb_context *ctx)
{
	if (p_libusb_unlock_event_waiters)
      p_libusb_unlock_event_waiters(ctx);
}

int libusb_wait_for_event(libusb_context *ctx, struct timeval *tv)
{
	return (p_libusb_wait_for_event) ? p_libusb_wait_for_event(ctx, tv) : LIBUSB_ERROR_OTHER;
}

int libusb_handle_events_timeout(libusb_context *ctx, struct timeval *tv)
{
	return (p_libusb_handle_events_timeout) ? p_libusb_handle_events_timeout(ctx, tv) : LIBUSB_ERROR_OTHER;
}

int libusb_handle_events_timeout_completed(libusb_context *ctx, struct timeval *tv, int *completed)
{
	return (p_libusb_handle_events_timeout_completed) ? p_libusb_handle_events_timeout_completed(ctx, tv, completed) : LIBUSB_ERROR_OTHER;
}

int libusb_handle_events(libusb_context *ctx)
{
	return (p_libusb_handle_events) ? p_libusb_handle_events(ctx) : LIBUSB_ERROR_OTHER;
}

int libusb_handle_events_completed(libusb_context *ctx, int *completed)
{
	return (p_libusb_handle_events_completed) ? p_libusb_handle_events_completed(ctx, completed) : LIBUSB_ERROR_OTHER;
}

int libusb_handle_events_locked(libusb_context *ctx, struct timeval *tv)
{
	return (p_libusb_handle_events_locked) ? p_libusb_handle_events_locked(ctx, tv) : LIBUSB_ERROR_OTHER;
}

int libusb_pollfds_handle_timeouts(libusb_context *ctx)
{
	return (p_libusb_pollfds_handle_timeouts) ? p_libusb_pollfds_handle_timeouts(ctx) : LIBUSB_ERROR_OTHER;
}

int libusb_get_next_timeout(libusb_context *ctx, struct timeval *tv)
{
	return (p_libusb_get_next_timeout) ? p_libusb_get_next_timeout(ctx, tv) : LIBUSB_ERROR_OTHER;
}

const struct libusb_pollfd ** libusb_get_pollfds(libusb_context *ctx)
{
	return (p_libusb_get_pollfds) ? p_libusb_get_pollfds(ctx) : NULL;
}

void libusb_free_pollfds(const struct libusb_pollfd **pollfds)
{
	if (p_libusb_free_pollfds)
      p_libusb_free_pollfds(pollfds);
}

void libusb_set_pollfd_notifiers(libusb_context *ctx, libusb_pollfd_added_cb added_cb, libusb_pollfd_removed_cb removed_cb, void *user_data)
{
	if (p_libusb_set_pollfd_notifiers)
      p_libusb_set_pollfd_notifiers(ctx, added_cb, removed_cb, user_data);
}
