/** @file connector_java.c
    @brief Contains the Java language bindings for the Substance Connector library.
    @author Galen Helfter - Allegorithmic
    @date 20190104
    @copyright Allegorithmic. All rights reserved.
*/

#include <stdlib.h>
#include <stdio.h>

#include <substance/connector/java/connector_java.h>
#include <substance/connector/connector.h>

static JavaVM *jvm = NULL;
static jclass connector_class = NULL;

static void connector_jv_trampoline(unsigned int context, const char *message_type,
                               const char *message);

static void connector_jv_trampoline(unsigned int context, const char *message_type,
                               const char *message)
{
    JNIEnv *env = NULL;
    jmethodID callback_id = NULL;
    int result = 0;
    jstring converted_message = NULL;

    /* Acquire current env */
    result = (*jvm)->GetEnv(jvm, (void **) &env, JNI_VERSION_1_6);

    if (result == JNI_EDETACHED)
    {
        /* Bind thread to Java */
        (*jvm)->AttachCurrentThread(jvm, (void **) &env, NULL);
    }

    /* Perform callback */
    callback_id = (*env)->GetStaticMethodID(env, connector_class,
                                            "externalCallback",
                                            "(I;Ljava/lang/String)V");

    converted_message = (*env)->NewStringUTF(env, message);

    (*env)->CallStaticVoidMethod(env, connector_class, callback_id,
                                 (jint) context, converted_message);

    /* Release bound thread */
    (*jvm)->DetachCurrentThread(jvm);
}

JNIEXPORT
jstring JNICALL
Java_com_substance_connector_Connector_native_1version(JNIEnv *env,
                                       jclass java_class)
{
    const char *version = NULL;

    version = substance_connector_version();
    return (*env)->NewStringUTF(env, version);
}

JNIEXPORT
jint JNICALL
Java_com_substance_connector_Connector_native_1init(JNIEnv *env,
                                    jclass java_class,
                                    jstring application_name)
{
    jint result = 0;
    const char* native_application_name =
        (*env)->GetStringUTFChars(env, application_name, NULL);

    fprintf(stdout, "Class init is run.\n");
    (*env)->GetJavaVM(env, &jvm);

    /* Fire up native initialization */
    result = (jint) substance_connector_init(native_application_name);

    /* Register trampoline for Connector to enter Java */
    substance_connector_add_trampoline(connector_jv_trampoline);

    (*env)->ReleaseStringUTFChars(env, application_name, NULL);
    native_application_name = NULL;

    return result;
}

JNIEXPORT
jint JNICALL
Java_com_substance_connector_Connector_native_1shutdown(JNIEnv *env, jclass java_class)
{
    jint result = 0;
    jvm = NULL;

    (*env)->GetJavaVM(env, &jvm);

    result = (jint) substance_connector_shutdown();

    return result;
}
