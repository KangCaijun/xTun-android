#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <jni.h>

#include "crypto.h"
#include "logger.h"
#include "tun.h"


#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))

static struct tundev *tun;

static JavaVM *jvm;
static jobject vpnServiceObj;
static jmethodID protect_socket;
static jboolean global;


int
protectSocket(int fd) {
    JNIEnv *env = NULL;

    if ((*jvm)->GetEnv(jvm, (void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    return (*env)->CallBooleanMethod(env, vpnServiceObj, protect_socket, fd);
}

static jboolean
init(JNIEnv *env, jobject thiz,
     jobject obj, jint fd, jint mtu, jboolean isGlobalProxy, jboolean verbose,
     jstring server, jstring password, jstring dns, jstring domain_path) {

    jboolean rc = JNI_FALSE;

    vpnServiceObj = (*env)->NewGlobalRef(env, obj);
    jclass vpnService = (*env)->GetObjectClass(env, vpnServiceObj);
    protect_socket = (*env)->GetMethodID(env, vpnService, "protectSocket", "(I)Z");

    (*env)->DeleteLocalRef(env, vpnService);

    const char *c_server = (*env)->GetStringUTFChars(env, server, NULL);
    const char *c_password = (*env)->GetStringUTFChars(env, password, NULL);
    const char *c_dns = (*env)->GetStringUTFChars(env, dns, NULL);
    const char *c_domain_path = (*env)->GetStringUTFChars(env, domain_path, NULL);

    if (crypto_init(c_password)) {
        logger_log(LOG_ERR, "Crypto init failed");
        goto clean;
    }

    global = isGlobalProxy;
    if (!isGlobalProxy) {
        if ( dns_init(c_domain_path)) {
            goto clean;
        }
    }

    tun = tun_alloc();
    if (tun_config(tun, fd, mtu, isGlobalProxy, verbose, c_server, c_dns)) {
        tun_free(tun);
    } else {
        rc = JNI_TRUE;
    }

clean:
    (*env)->ReleaseStringUTFChars(env, server, c_server);
    (*env)->ReleaseStringUTFChars(env, password, c_password);
    (*env)->ReleaseStringUTFChars(env, dns, c_dns);
    (*env)->ReleaseStringUTFChars(env, domain_path, c_domain_path);

    return rc;
}

static void
start(JNIEnv *env, jobject thiz) {
    tun_start(tun);
}

static void
stop(JNIEnv *env, jobject thiz) {
    tun_stop(tun);
    if (!global) {
        dns_destroy();
    }
}

static int
jniRegisterNativeMethods(JNIEnv *env, const char *className,
                             const JNINativeMethod *methods, int numMethods) {
    int rc = 0;

    logger_log(LOG_INFO, "Registering %s natives", className);

    jclass cls = (*env)->FindClass(env, className);
    if(cls == NULL){
        logger_log(LOG_ERR, "Native registration unable to find class %s", className);
        return -1;
    }

    if((*env)->RegisterNatives(env, cls, methods, numMethods) < 0){
        logger_log(LOG_ERR, "RegisterNatives failed: %s", className);
        rc = -1;
    }

    (*env)->DeleteLocalRef(env, cls);

    return rc;
}

static const char *classPathName = "io/github/xTun/xTun";
static JNINativeMethod methods[] = {
    { "init", "(Lio/github/xTun/service/XtunVpnService;IIZZLjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", (void*)init },
    { "start", "()V", (void*)start },
    { "stop", "()V", (void*)stop },
};

jint
JNI_OnLoad(JavaVM *vm, void *reserved) {
    jvm = vm;
    JNIEnv *env = NULL;
    if ((*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    jniRegisterNativeMethods(env, classPathName, methods, NELEM(methods));
    return JNI_VERSION_1_6;
}

jint
JNI_OnUnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if ((*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    if (vpnServiceObj) {
        (*env)->DeleteGlobalRef(env, vpnServiceObj);
        vpnServiceObj = NULL;
    }
}
