
/**
 * POC for TBB threading from the JVM.
 * Running main will fire off multiple TBB threads using parallel_for which then call back into the Java callback method
 * This will print confirmation of the callback being called on different threads.
 */
public class TestThread {
	
	/**
	 * Callback - just show the start and end of range and the thread id
	 *
	 * @param s the start
	 * @param e the end (exclusive)
	 */
	public void callback(int s, int e) {
		synchronized(this) {
			System.out.printf("Callback {%d} %d->%d\n", Thread.currentThread().getId(), s, e);
		}
			
		try {
			Thread.sleep(500);
		} catch (InterruptedException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
	}
	
	public static void main(String args[]) {
		TestThread tt = new TestThread();
		
		NativeThread nt = new NativeThread();
		nt.fire(tt);
	}
}
