/*
 *  <:copyright-BRCM:2016-2020:Apache:standard
 *  
 *   Copyright (c) 2016-2020 Broadcom. All Rights Reserved
 *  
 *   The term “Broadcom” refers to Broadcom Inc. and/or its subsidiaries
 *  
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *  
 *       http://www.apache.org/licenses/LICENSE-2.0
 *  
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *  
 *  :>
 *
 *****************************************************************************/

#ifndef _ONU_MGMT_MODEL_HELPERS_H
#define _ONU_MGMT_MODEL_HELPERS_H

#include <bcmos_system.h>
#include <bcmolt_mgt_group.h>
#include <onu_mgmt_model_ids.h>

typedef enum bcmonu_mgmt_msg_type
{
    BCMONU_MGMT_MSG_TYPE_GET = 0x01,
    BCMONU_MGMT_MSG_TYPE_SET = 0x02,
    BCMONU_MGMT_MSG_TYPE_CLEAR = 0x04,
    BCMONU_MGMT_MSG_TYPE_MULTI = 0x08,
    BCMONU_MGMT_MSG_TYPE_GET_MULTI = BCMONU_MGMT_MSG_TYPE_GET | BCMONU_MGMT_MSG_TYPE_MULTI,
} bcmonu_mgmt_msg_type;

typedef enum bcmonu_mgmt_msg_dir
{
    BCMONU_MGMT_MSG_DIR_REQUEST,
    BCMONU_MGMT_MSG_DIR_RESPONSE
} bcmonu_mgmt_msg_dir;

/** @note presence_mask set to 0 means all properties */
#define _BCMONU_MGMT_REQ_INIT(_h, _obj, _grp, _subgrp) \
    (_h)->err = BCM_ERR_OK; \
    (_h)->obj_type = bcmonu_mgmt_obj_id_ ## _obj; \
    (_h)->group = _grp; \
    (_h)->subgroup = _subgrp;

/* General (called by the autogenerated code of the CLI handlers). */
#define BCMONU_MGMT_CFG_INIT(_s, _obj, _key) \
    do \
    { \
        bcmonu_mgmt_ ## _obj ## _cfg *_x_ = _s; \
        memset(_x_, 0, sizeof(*_x_)); \
        _BCMONU_MGMT_REQ_INIT(&((_x_)->hdr.hdr), _obj, BCMOLT_MGT_GROUP_CFG, 0); \
        (_x_)->key = _key; \
    } while (0)


/** @note the FIELD related macros are used in Aspen auto-generated code for field set/get */
/** Indicate that an API field is present (set/requested) */
#define BCMONU_MGMT_FIELD_SET_PRESENT(_s_ptr, _s_type, _f_name) \
    (_s_ptr)->presence_mask |= (1ULL << bcmonu_mgmt_ ## _s_type ## _id_ ## _f_name)

/** Set API field value */
#define BCMONU_MGMT_FIELD_SET(_s_ptr, _s_type, _f_name, _field_value) \
    do {                                                              \
        BCMONU_MGMT_FIELD_SET_PRESENT(_s_ptr, _s_type, _f_name);      \
        (_s_ptr)->_f_name = (_field_value);                           \
    } while(0)

/** Check if API field is present (set/requested) */
#define BCMONU_MGMT_FIELD_IS_SET(_s_ptr, _s_type, _f_name)                                  \
    ((_s_ptr)->presence_mask == 0 ||                                                        \
     ((_s_ptr)->presence_mask & (1ULL << bcmonu_mgmt_ ## _s_type ## _id_ ## _f_name)) != 0)

#define BCMONU_MGMT_FIELD_MASK_GET(_s_ptr, _s_type, _f_name)                                  \
    (1ULL << bcmonu_mgmt_ ## _s_type ## _id_ ## _f_name)

#define BCMONU_MGMT_FIELD_IS_SET_FOR_ALL_PROPERTIES(_s_ptr) \
    ((_s_ptr)->presence_mask == 0)


/** Set a key field to a wildcard value, meaning it will be ignored when performing a multi-object API.
 *
 * \param[in]   _f_ptr  Pointer to the field to set
 */
#define BCMONU_MGMT_KEY_SET_TO_WILDCARD(_f_ptr) memset((_f_ptr), 0xFF, sizeof(*(_f_ptr)))

/** Checks if a key field is set to a wildcard value, meaning it will be ignored when performing a multi-object API.
 *
 * \param[in]   _f_ptr  Pointer to the field to check
 */
#define BCMONU_MGMT_KEY_IS_WILDCARD(_f_ptr) bcmonu_mgmt_key_is_wildcard((const uint8_t *)(_f_ptr), sizeof(*(_f_ptr)))

/** Internal helper function for BCMONU_MGMT_KEY_IS_WILDCARD. */
static inline bcmos_bool bcmonu_mgmt_key_is_wildcard(const uint8_t *f_ptr, size_t size)
{
    size_t i;
    for (i = 0; i < size; i++)
    {
        if (f_ptr[i] != 0xFF)
        {
            return BCMOS_FALSE;
        }
    }
    return BCMOS_TRUE;
}

/** Array element access
 */

/* Check if array structure has a dynamically allocated "arr" field */
#define _BCMONU_MGMT_ARRAY_IS_DYNAMIC(_a_ptr)                                        \
    ((const void *)(_a_ptr) != (const void *)(&((_a_ptr)->arr_index_mask)))

/* Check if array index is valid */
#define _BCMONU_MGMT_ARRAY_ELEM_IDX_IS_VALID(_a_ptr, _idx)                           \
    (_BCMONU_MGMT_ARRAY_IS_DYNAMIC(_a_ptr) ||                                        \
     ((int)(_idx) * sizeof((_a_ptr)->arr[0]) < sizeof((_a_ptr)->arr)))

/* Check if array index is in bounds */
#define _BCMONU_MGMT_ARRAY_ELEM_IDX_IS_IN_BOUNDS(_a_ptr, _idx)                        \
    (_BCMONU_MGMT_ARRAY_ELEM_IDX_IS_VALID(_a_ptr, _idx) &&                            \
     (_BCMONU_MGMT_ARRAY_IS_DYNAMIC(_a_ptr) || (int)(_idx) < sizeof((_a_ptr)->arr_index_mask) * 8))

/** Set presence flag for array element.
 * Note: array indices >63 cannot be covered by the index mask and are assumed to always be set.
 *
 * \param[in]   _a_ptr  pointer to the array attribute/field structure.
 *              the structure contains 2 fields: arr_index_mask and array "arr"
 * \param[in]   _idx    array element index
 * \returns     BCM_ERR_OK if it is valid
 *              BCM_ERR_RANGE (<0) if index is out of range
 */
#define BCMONU_MGMT_ARRAY_ELEM_SET_PRESENT(_a_ptr, _idx)                             \
    (!_BCMONU_MGMT_ARRAY_ELEM_IDX_IS_IN_BOUNDS(_a_ptr, _idx) ? BCM_ERR_RANGE :       \
     (((int)(_idx) > 63) ? BCM_ERR_OK :                                         \
      ((_a_ptr)->arr_index_mask |= (1ULL << (int)(_idx)), BCM_ERR_OK)))

/** Check if array element is present.
 * Note: array indices >63 cannot be covered by the index mask and are assumed to always be set.
 *
 * \param[in]   _a_ptr  pointer to the array attribute/field structure.
 *              the structure contains 2 fields: arr_index_mask and array "arr"
 * \param[in]   _idx    array element index
 * \returns     BCMOS_TRUE if element is present
 */
#define BCMONU_MGMT_ARRAY_ELEM_IS_PRESENT(_a_ptr, _idx)                              \
    (_BCMONU_MGMT_ARRAY_ELEM_IDX_IS_VALID(_a_ptr, _idx) &&                           \
     ((int)(_idx) > 63 ||                                                       \
      (_a_ptr)->arr_index_mask == 0 ||                                          \
      (((_a_ptr)->arr_index_mask & (1ULL << (int)(_idx))) != 0)))

/** Set array element
 * \param[in]   _a_ptr  pointer to the array attribute/field structure.
 *              the structure contains 2 fields: arr_index_mask and array "arr"
 * \param[in]   _idx    array element index
 * \param[in]   _val    element value to be set
 * \returns     BCM_ERR_OK if it is valid
 *              BCM_ERR_RANGE (<0) if index is out of range
 *              BCM_ERR_NULL (<0) if "arr" was null (for dynamically allocated arrays)
 */
#define BCMONU_MGMT_ARRAY_ELEM_SET(_a_ptr, _idx, _val)                                   \
    ((_BCMONU_MGMT_ARRAY_IS_DYNAMIC(_a_ptr) && ((_a_ptr)->arr == NULL)) ? BCM_ERR_NULL : \
     ((BCMONU_MGMT_ARRAY_ELEM_SET_PRESENT(_a_ptr, _idx) != BCM_ERR_OK) ? BCM_ERR_RANGE : \
      ((_a_ptr)->arr[_idx] = _val, BCM_ERR_OK)))


/*
 * Macros for multi-object configuration and statistics retrieval
 */

/** Allocate a multi-object configuration API structure.
 *  This allocates memory - once the structure is no longer in use, it must be freed with bcmolt_multi_msg_free().
 *
 * \param[in]   _obj            Object type
 * \param[in]   _key            Object key
 * \param[in]   _max_responses  Maximum number of response objects per API call
 */
#define BCMONU_MGMT_MULTI_CFG_ALLOC(_obj, _key, _max_responses) \
    ((bcmonu_mgmt_ ## _obj ## _multi_cfg *) \
        bcmonu_mgmt_multi_msg_alloc(bcmonu_mgmt_obj_id_ ## _obj, BCMOLT_MGT_GROUP_CFG, 0, &(_key), (_max_responses)))



/** @todo multi cfg get feature is not supported for onu mgmt */
static inline void *bcmonu_mgmt_multi_msg_alloc(
    bcmonu_mgmt_obj_id obj_id,
    bcmolt_mgt_group group,
    uint16_t subgroup,
    const void *key,
    uint16_t max_responses)
{
    return NULL;
}




/* Flow */
/** @note flow prop macros are being used for set/get of 'match' or 'action' fields and their sub-fields.
    These macros check if the 'action' itself is set in flow cfg data presence_mask,
    and then checks if the 'action type' bit mask is set in the action field presence_mask.
    Similar mechanism for Set operation.
    */
#define BCMONU_MGMT_FLOW_PROP_IS_SET(_m, _sub, _p)              \
    (BCMONU_MGMT_FIELD_IS_SET((_m), flow_cfg_data, _sub) &&    \
    BCMONU_MGMT_FIELD_IS_SET(&((_m)->_sub), flow_ ## _sub, _p))

#define BCMONU_MGMT_FLOW_PROP_SET(_m, _sub, _p, _v)                     \
    do                                                                  \
    {                                                                   \
         BCMONU_MGMT_FIELD_SET_PRESENT((_m), flow_cfg_data, _sub);     \
         BCMONU_MGMT_FIELD_SET(&((_m)->_sub), flow_ ## _sub, _p, _v);   \
    } while (0)



#define bcmonu_mgmt_flow_action_type_id_push BCMONU_MGMT_FLOW_ACTION_TYPE_ID_PUSH
#define bcmonu_mgmt_flow_action_type_id_pop BCMONU_MGMT_FLOW_ACTION_TYPE_ID_POP
#define bcmonu_mgmt_flow_action_type_id_translate_pcp BCMONU_MGMT_FLOW_ACTION_TYPE_ID_TRANSLATE_PCP
#define bcmonu_mgmt_flow_action_type_id_translate_vid BCMONU_MGMT_FLOW_ACTION_TYPE_ID_TRANSLATE_VID
#define bcmonu_mgmt_flow_action_type_id_push_inner_tag BCMONU_MGMT_FLOW_ACTION_TYPE_ID_PUSH_INNER_TAG
#define bcmonu_mgmt_flow_action_type_id_pop_inner_tag BCMONU_MGMT_FLOW_ACTION_TYPE_ID_POP_INNER_TAG
#define bcmonu_mgmt_flow_action_type_id_translate_inner_pcp BCMONU_MGMT_FLOW_ACTION_TYPE_ID_TRANSLATE_INNER_PCP
#define bcmonu_mgmt_flow_action_type_id_translate_inner_vid BCMONU_MGMT_FLOW_ACTION_TYPE_ID_TRANSLATE_INNER_VID

#define ONU_MGMT_FLOW_IS_UNICAST(cfg)      (BCMONU_MGMT_FIELD_IS_SET(&(cfg)->data, flow_cfg_data, flow_type) &&\
                                            (BCMONU_MGMT_FLOW_TYPE_UNICAST == (cfg)->data.flow_type))

#define ONU_MGMT_FLOW_IS_MULTICAST(cfg)    (BCMONU_MGMT_FIELD_IS_SET(&(cfg)->data, flow_cfg_data, flow_type) &&\
                                            (BCMONU_MGMT_FLOW_TYPE_MULTICAST == (cfg)->data.flow_type))

#define ONU_MGMT_FLOW_IS_BROADCAST(cfg)    (BCMONU_MGMT_FIELD_IS_SET(&(cfg)->data, flow_cfg_data, flow_type) &&\
                                            (BCMONU_MGMT_FLOW_TYPE_BROADCAST == (cfg)->data.flow_type))

/**
  Macro to check for a flow which is configured to match untagged upstream packets
  through ONU UNI and also forwarded to ANI side as untagged
  (i.e. no action to insert VID or PCP).
  Vice versa for downstream packets.
  */
#define ONU_MGMT_FLOW_IS_UNTAGGED_END_TO_END(cfg_data)\
        (BCMONU_MGMT_FLOW_PROP_IS_SET(cfg_data, match, o_untagged) &&\
        (!BCMONU_MGMT_FLOW_PROP_IS_SET(cfg_data, action, o_vid) &&\
         !BCMONU_MGMT_FLOW_PROP_IS_SET(cfg_data, action, o_pcp)))

#define BCMONU_MGMT_FLOW_ACTION_TYPE_MASK_GET(_p) bcmonu_mgmt_flow_ ## action_type ## _id_ ## _p

/* IGMP action on Upstream */
#define BCMONU_MGMT_IGMP_US_PROP_MASK_GET(_sub, _p) bcmonu_mgmt_igmp_us_ ## _sub ## _id_ ## _p

/**
 * @todo See comment above to make it one set of macros for flow action and igmp action.
 */
#define _BCMONU_MGMT_IGMP_US_PROP_SET_PRESENT(_m, _sub, _p) \
    do \
    { \
        (_m)->_sub.presence_mask |= BCMONU_MGMT_IGMP_US_PROP_MASK_GET(_sub, _p); \
    } while (0)

#define _BCMONU_MGMT_IGMP_US_PROP_IS_PRESENT(_m, _sub, _p) \
    (((_m)->_sub.presence_mask & BCMONU_MGMT_IGMP_US_PROP_MASK_GET(_sub, _p)) ? BCMOS_TRUE : BCMOS_FALSE)

#define BCMONU_MGMT_IGMP_US_PROP_IS_SET(_m, _sub, _p) _BCMONU_MGMT_IGMP_US_PROP_IS_PRESENT(_m, _sub, _p)

#define BCMONU_MGMT_IGMP_US_PROP_SET(_m, _sub, _p, _v) \
    do \
    { \
         _BCMONU_MGMT_IGMP_US_PROP_SET_PRESENT(_m, _sub, _p); \
         (_m)->_sub._p = (_v); \
    } while (0)

#define BCMONU_MGMT_IGMP_US_PROP_OR_SET(_m, _sub, _p, _v) BCMONU_MGMT_IGMP_US_PROP_SET(_m, _sub, _p, (_m)->_sub._p | _v)

/* IGMP upstream action type */
#define bcmonu_mgmt_igmp_us_action_type_id_add_vlan_tag  BCMONU_MGMT_IGMP_US_ACTION_TYPE_ID_ADD_VLAN_TAG
#define bcmonu_mgmt_igmp_us_action_type_id_replace_tci   BCMONU_MGMT_IGMP_US_ACTION_TYPE_ID_REPLACE_TCI
#define bcmonu_mgmt_igmp_us_action_type_id_replace_vid   BCMONU_MGMT_IGMP_US_ACTION_TYPE_ID_REPLACE_VID


#define MAX_VID 4095

/** @brief proxy type and packet, used for OMCI control path messaging */
typedef enum
{
    ONU_MGMT_REMOTE_PROXY_RX_PKT = 0,
    ONU_MGMT_DIRECT_PROXY_RX_PKT_GPON,
    ONU_MGMT_DIRECT_PROXY_RX_PKT_XGPON
} onu_mgmt_proxy_rx_pkt_type;

#endif /* _ONU_MGMT_MODEL_HELPERS_H */