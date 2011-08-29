
/**
 * This defines a class with a native implementation. 
 * 
 * Calling 'fire' on an instance of this class will call Java_NativeThread_fire
 * This uses the TBB parallel_for operation to call {@link TestThread#callback(int, int)} on multiple threads
 * 
 */
public class NativeThread {
	native void fire(TestThread callbacker);
	
	static {
		// Load the native library (libnativethread.dylib)
		// Also need libtbb.dylib in the same directory
		System.loadLibrary("nativethread");
	}
}
