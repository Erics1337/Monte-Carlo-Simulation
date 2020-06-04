import java.util.ArrayList;

public class Main {
    private static double T;
    private static double N;
    Object lock = new Object();
    double nSuccess = 0;
    double value;
    double x = 0, y = 0;


    public static void main(String[] args) {
        T = Integer.parseInt(args[0]);
        N = Integer.parseInt(args[1]);
        //error check input
        if (T < 0 || T > 1000 || N < 0 || N > 10000){
            System.out.println("Input Error");
            System.exit(0);
        }
        new Main().go();
    }


    private void go() {
        ArrayList<Thread> threads = new ArrayList<>();
        for (int t = 1; t <= this.T; t++) {
            for (int i = 1; i <= this.N; i++) {
                Runnable task = new Task();
                Thread thread = new Thread(task);
                threads.add(thread);
                thread.start();
            }
        }
        //main thread waits for all threads to finish
        for (Thread t : threads) {
            try {
                t.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        value = 4 * nSuccess / (N * T);
        System.out.println("Threads = " + T);
        System.out.println("Throws = " + N);
        System.out.println("Successful = " + nSuccess);
        System.out.println("Calculated pi = " + value);
        System.out.println("Difference to exact value of pi: " + (value - Math.PI));
    }


    class Task implements Runnable {
        @Override   //inner class
        public void run() {
            synchronized (lock) { //makes atomic object to prevent deadlock of more atomic assembly commands
                x = Math.random();
                y = Math.random();
                if (Math.sqrt(x * x + y * y) <= 1)
                    nSuccess++;

            }
        }
    }
}

