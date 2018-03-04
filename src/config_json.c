
#include <json-c/json.h>
#include "config_json.h"
#include "ssr_executive.h"

bool json_iter_extract_string(const char *key, const struct json_object_iter *iter, const char **value) {
    bool result = false;
    do {
        if (key == NULL || iter == NULL || value == NULL) {
            break;
        }
        *value = NULL;
        if (strcmp(iter->key, key) != 0) {
            break;
        }
        struct json_object *val = iter->val;
        if (json_type_string != json_object_get_type(val)) {
            break;
        }
        *value = json_object_get_string(val);
        result = true;
    } while (0);
    return result;
}

bool json_iter_extract_int(const char *key, const struct json_object_iter *iter, int *value) {
    bool result = false;
    do {
        if (key == NULL || iter == NULL || value == NULL) {
            break;
        }
        *value = 0;
        if (strcmp(iter->key, key) != 0) {
            break;
        }
        struct json_object *val = iter->val;
        if (json_type_int != json_object_get_type(val)) {
            break;
        }
        *value = json_object_get_int(val);
        result = true;
    } while (0);
    return result;
}

bool json_iter_extract_bool(const char *key, const struct json_object_iter *iter, bool *value) {
    bool result = false;
    do {
        if (key == NULL || iter == NULL || value == NULL) {
            break;
        }
        *value = false;
        if (strcmp(iter->key, key) != 0) {
            break;
        }
        struct json_object *val = iter->val;
        if (json_type_boolean != json_object_get_type(val)) {
            break;
        }
        *value = (bool)json_object_get_boolean(val);
        result = true;
    } while (0);
    return result;
}

bool parse_config_file(const char *file, struct server_config *config) {
    bool result = false;
    json_object *jso = NULL;
    do {
        jso = json_object_from_file(file);
        if (jso == NULL) {
            break;
        }
        struct json_object_iter iter;
        json_object_object_foreachC(jso, iter) {
            int obj_int = 0;
            bool obj_bool = false;
            const char *obj_str = NULL;
            if (json_iter_extract_string("local_address", &iter, &obj_str)) {
                string_safe_assign(&config->listen_host, obj_str);
                continue;
            }
            if (json_iter_extract_int("local_port", &iter, &obj_int)) {
                config->listen_port = obj_int;
                continue;
            }
            if (json_iter_extract_string("server", &iter, &obj_str)) {
                string_safe_assign(&config->remote_host, obj_str);
                continue;
            }
            if (json_iter_extract_int("server_port", &iter, &obj_int)) {
                config->remote_port = obj_int;
                continue;
            }
            if (json_iter_extract_string("password", &iter, &obj_str)) {
                string_safe_assign(&config->password, obj_str);
                continue;
            }
            if (json_iter_extract_string("method", &iter, &obj_str)) {
                string_safe_assign(&config->method, obj_str);
                continue;
            }
            if (json_iter_extract_string("protocol", &iter, &obj_str)) {
                if (obj_str && strcmp(obj_str, "verify_sha1") == 0) {
                    // LOGI("The verify_sha1 protocol is deprecate! Fallback to origin protocol.");
                    obj_str = NULL;
                }
                string_safe_assign(&config->protocol, obj_str);
                continue;
            }
            if (json_iter_extract_string("protocol_param", &iter, &obj_str)) {
                string_safe_assign(&config->protocol_param, obj_str);
                continue;
            }
            if (json_iter_extract_string("obfs", &iter, &obj_str)) {
                string_safe_assign(&config->obfs, obj_str);
                continue;
            }
            if (json_iter_extract_string("obfs_param", &iter, &obj_str)) {
                string_safe_assign(&config->obfs_param, obj_str);
                continue;
            }
            if (json_iter_extract_int("timeout", &iter, &obj_int)) {
                config->idle_timeout = obj_int * SECONDS_PER_MINUTE;
                continue;
            }
            if (json_iter_extract_bool("udp", &iter, &obj_bool)) {
                config->udp = obj_bool;
                continue;
            }
        }
        result = true;
    } while (0);
    if (jso) {
        json_object_put(jso);
    }
    return result;
}
