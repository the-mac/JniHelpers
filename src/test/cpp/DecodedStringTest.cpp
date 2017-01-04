/*
 * Copyright (c) 2014 Spotify AB
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "DecodedStringTest.h"
#include "JUnitUtils.h"
#include "DecodedString.h"
#include "TestObject.h"

using namespace operators::nativity::decodedstring;

void DecodedStringTest::initialize(JNIEnv *env) {
  setClass(env);

  DecodedString decodedString;
  const char* decodedStringName = decodedString.getCanonicalName();

  addNativeMethod("createDecodedString", (void*)&createDecodedString, decodedStringName, NULL);
  addNativeMethod("getPersistedInstance", (void*)&getPersistedInstance, decodedStringName, decodedStringName, NULL);
  addNativeMethod("nativeIsPersistenceEnabled", (void*)nativeIsPersistenceEnabled, kTypeVoid, NULL);
  addNativeMethod("isPersistenceEnabledWithoutInit", (void*)isPersistenceEnabledWithoutInit, kTypeVoid, NULL);
  addNativeMethod("destroyDecodedString", (void*)&destroyDecodedString, kTypeVoid, decodedStringName, NULL);
  addNativeMethod("persistNullObject", (void*)&persistNullObject, kTypeVoid, NULL);
  addNativeMethod("destroyInvalidClass", (void*)&destroyInvalidClass, kTypeVoid, NULL);
  addNativeMethod("destroyNullObject", (void*)&destroyNullObject, kTypeVoid, NULL);

  registerNativeMethods(env);
}

jobject DecodedStringTest::createDecodedString(JNIEnv *env, jobject javaThis) {
  LOG_INFO("Starting test: createDecodedString");
  DecodedString *decodedString = new DecodedString(env);
  decodedString->encodedString = TEST_ENCODE;
  // Persist should be called for us here. Note that the original object is leaked; it will
  // be cleaned up in destroyDecodedString().
  return decodedString->toJavaObject(env);
}

jobject DecodedStringTest::getPersistedInstance(JNIEnv *env, jobject javaThis, jobject object) {
  LOG_INFO("Starting test: getPersistedInstance");
  ClassRegistry registry;
  registry.add(env, new DecodedString(env));
  DecodedString *decodedString = registry.getNativeInstance<DecodedString>(env, object);
  JUNIT_ASSERT_EQUALS_STRING(TEST_ENCODE, decodedString->encodedString.get());
  JUNIT_ASSERT_NOT_NULL(decodedString->getCanonicalName());
  JUNIT_ASSERT_TRUE(decodedString->isInitialized());
  return decodedString->toJavaObject(env);
}

void DecodedStringTest::nativeIsPersistenceEnabled(JNIEnv *env, jobject javaThis) {
  LOG_INFO("Starting test: nativeIsPersistenceEnabled");
  DecodedString decodedString(env);
  JUNIT_ASSERT_TRUE(decodedString.isInitialized());
  DecodedString mergedObject;
  mergedObject.merge(&decodedString);
  JUNIT_ASSERT_TRUE(mergedObject.isInitialized());
}

void DecodedStringTest::isPersistenceEnabledWithoutInit(JNIEnv *env, jobject javaThis) {
  DecodedString decodedString;
  JUNIT_ASSERT_FALSE(decodedString.isInitialized());
}

void DecodedStringTest::destroyDecodedString(JNIEnv *env, jobject javaThis, jobject object) {
  LOG_INFO("Starting test: destroyDecodedString");
  ClassRegistry registry;
  registry.add(env, new DecodedString(env));
  DecodedString *decodedString = registry.getNativeInstance<DecodedString>(env, object);
  decodedString->destroy(env, object);
}

void DecodedStringTest::persistNullObject(JNIEnv *env, jobject javaThis) {
  LOG_INFO("Starting test: persistNullObject");
  DecodedString decodedString(env);
  decodedString.mapFields();
  JUNIT_ASSERT_FALSE(decodedString.persist(env, NULL));
}

void DecodedStringTest::destroyInvalidClass(JNIEnv *env, jobject javaThis) {
  // This test is almost impossible to replicate from Java, and frankly should
  // not happen from (responsible) C++ code either. It would be possible to catch
  // if we are willing to do fieldID lookups on the fly rather than cached, but
  // that assumes that performance is not an issue here. For that reason, this
  // test is excluded and the erroneous behavior will (and probably should) crash
  // the JVM if enabled.
#if 0
  LOG_INFO("Starting test: destroyInvalidClass");
  DecodedString decodedString(env);
  decodedString.mapFields();
  decodedString.destroy(env, javaThis);
#endif
}

void DecodedStringTest::destroyNullObject(JNIEnv *env, jobject javaThis) {
  LOG_INFO("Starting test: destroyNullObject");
  DecodedString decodedString(env);
  decodedString.mapFields();
  decodedString.destroy(env, NULL);
}
