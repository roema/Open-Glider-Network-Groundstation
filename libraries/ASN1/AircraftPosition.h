/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "AircraftModul"
 * 	found in "ogn.asn1"
 * 	`asn1c -fnative-types`
 */

#ifndef	_AircraftPosition_H_
#define	_AircraftPosition_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <BOOLEAN.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* AircraftPosition */
typedef struct AircraftPosition {
	long	 callsign;
	long	 timestamp;
	long	 lat;
	long	 lon;
	long	 alt;
	long	 type;
	BOOLEAN_t	 stealth;
	BOOLEAN_t	 notrack;
	long	 course;
	long	 heading;
	long	 spped;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} AircraftPosition_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_AircraftPosition;

#ifdef __cplusplus
}
#endif

#endif	/* _AircraftPosition_H_ */
#include <asn_internal.h>
