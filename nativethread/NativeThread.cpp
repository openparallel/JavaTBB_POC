/*
 * NativeThread.cpp
 *
 *  Created on: Aug 28, 2011
 *      Author: richard.parratt@openparallel.com
 */
#include <jni.h>
#include <stdio.h>
#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"

#include "NativeThread.h"	// Generated by javah

using namespace tbb;

//
// This is the body class that TBB's parallel_for uses.
// It creates an instance of this, passes it parameters and calls operator() on each thread.
//
class PforBody {

public:
	jobject testThread;		// The Java TestThread object that was passed to the NativeThread.fire call
	JNIEnv *javaEnv;		// Java's environment (valid only for the thread that called 'fire', not actually used)
	JavaVM *jvm;			// The single JVM instance

	// Construct a new instance with the Java environment pointer 'env' and TestThread object tThread
	PforBody(JNIEnv *env, jobject tThread) {
		javaEnv = env;
		javaEnv->GetJavaVM(&jvm);
		testThread = tThread;
	}

	// Processing operator - in real life processes data for the range defined by 'r'
	//
	// This just calls TestThread.callback(r.begin(), r.end())
	// Designed to be called on any thread
	//
	void operator() (const blocked_range<int>& r ) const {
		// printf("operator() %d->%d\n", r.begin(), r.end());

		// Attach the current thread to the JVM, allowing access to the JVM environment through 'thrdEnv'
		JNIEnv *thrdEnv;
		jvm->AttachCurrentThread ((void **) &thrdEnv, NULL);

		// Get the TestThread instance as a jclass
		jclass cls = thrdEnv->GetObjectClass(testThread);
		if(cls==0) {
			fprintf(stderr, "GetObjectClass returned null\n");
			return;
		}

		// Get the method id for 'void TestThread.callback(int, int)'
		jmethodID mid = thrdEnv->GetMethodID(cls, "callback", "(II)V");
		if(mid==0) {
			fprintf(stderr, "GetMethodID returned null\n");
			return;
		}

		// Now call the actual method
		jint s = r.begin();
		jint e = r.end();
		thrdEnv->CallVoidMethod(testThread, mid, s, e );

		// And detach our thread
		jvm->DetachCurrentThread();
	};
};

// This implements NativeThread.fire in C++
// We just call parallel_for for a range of 1->10 exclusive
JNIEXPORT void JNICALL Java_NativeThread_fire(JNIEnv *env, jobject nThread, jobject tThread) {

	// printf("NativeThread_fire\n");

	 /* Initialise the TBB runtime if it hasn't already happened */
	task_scheduler_init init;

	// Fire off parallel operation on range
	parallel_for(blocked_range<int>(0,10), PforBody(env, tThread));

}


