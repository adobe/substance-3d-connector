/** @file link_java.h
    @brief Include header defining the static native methods to bind connector
    @author Galen Helfter - Allegorithmic
    @date 20190104
    @copyright Allegorithmic. All rights reserved.
*/

#include <jni.h>

#ifndef _SUBSTANCE_LINK_JAVA_LINK_JAVA_H
#define _SUBSTANCE_LINK_JAVA_LINK_JAVA_H

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/* com.alg.connector.Connector.native_version static method. Returns a string
 * for the current version. */
JNIEXPORT
jstring JNICALL
Java_com_alg_link_Link_native_1version(JNIEnv *env,
                                       jclass java_class);

/* com.alg.connector.Connector.native_init static method. Returns an integer
 * representing the success of initialization. */
JNIEXPORT
jint JNICALL
Java_com_alg_link_Link_native_1init(JNIEnv *env,
                                    jclass java_class,
                                    jstring application_name);

/* com.alg.connector.Connector.native_shutdown static method. Returns an integer
 * representing the sucess of the shutdown call. */
JNIEXPORT
jint JNICALL
Java_com_alg_link_Link_native_1shutdown(JNIEnv *env, jclass java_class);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _SUBSTANCE_LINK_JAVA_LINK_JAVA_H */
