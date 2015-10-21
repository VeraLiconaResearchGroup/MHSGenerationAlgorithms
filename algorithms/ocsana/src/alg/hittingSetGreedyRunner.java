import java.util.*;
import java.util.stream.*;

import java.io.*;

public class hittingSetGreedyRunner {
    public static void main (String[] args) throws Exception {
        // Handle arguments
        if (args.length < 2 || args.length > 3) {
            System.out.println("Two required arguments: infile outfile; one optional: cutoff");
            System.exit(1);
        }

        String infile = args[0];
        String outfile = args[1];

        Integer cutoff = Integer.MAX_VALUE;

        if (args.length == 3) {
            cutoff = Integer.valueOf(args[2]);
        }

        // Process input
        setsWithMaxElement inSets = setsFromFile(infile);

        // Generate dummy values for OCSANA variables
        List<Integer> elementaryNodesList = new ArrayList<Integer>();
        for (int i = 0; i <= inSets.maxElement; i++) {
            elementaryNodesList.add(i);
        }

        ArrayList<Float> nodeOCSANAScoresList = new ArrayList<Float>();
        for (int i = 0; i <= inSets.maxElement; i++) {
            nodeOCSANAScoresList.add(1f);
        }

        int[] nodeSetScores = new int[inSets.maxElement + 1];
        for (int i = 0; i <= inSets.maxElement; i++) {
            nodeSetScores[i] = 1;
        }

        // Run the algorithm
        hittingSetGreedyAlgorithm algRun =
            new hittingSetGreedyAlgorithm(cutoff,
                                          Double.MAX_VALUE,
                                          inSets.sets,
                                          nodeOCSANAScoresList,
                                          nodeSetScores,
                                          elementaryNodesList);
        List<List<Integer>> result = algRun.getCIset();

        // Print the result
        System.out.println("Found " + result.size() + " hitting sets.");
        writeFileFromSets(outfile, result);
    }

    private static setsWithMaxElement setsFromFile(String fileName)
        throws FileNotFoundException, NumberFormatException {
        List<List<Integer>> sets = new ArrayList<List<Integer>>();
        Integer maxElement = 0;

        Scanner inFileScanner = new Scanner(new File(fileName));

        while (inFileScanner.hasNextLine()) {
            String line = inFileScanner.nextLine();
            String[] lineWords = line.split(" ");

            List<Integer> lineList = new ArrayList<Integer>();
            for (String word: lineWords) {
                lineList.add(Integer.valueOf(word));
            }

            sets.add(lineList);

            Integer lineMax = Collections.max(lineList);
            if (lineMax > maxElement) {
                maxElement = lineMax;
            }
        }

        setsWithMaxElement result = new setsWithMaxElement(sets, maxElement);
        return result;
    }

    private static void writeFileFromSets(String fileName, List<List<Integer>> sets)
        throws IOException {
        FileWriter outFile = new FileWriter(fileName);

        for (List<Integer> set: sets) {
            Collections.sort(set);
            String line = new String();
            for (Integer elt: set) {
                line += elt + " ";
            }
            line += "\n";
            outFile.write(line);
        }

        outFile.close();
    }

    private static class setsWithMaxElement {
        final List<List<Integer>> sets;
        final Integer maxElement;
        setsWithMaxElement(List<List<Integer>> sets, Integer maxElement) {
            this.sets = sets;
            this.maxElement = maxElement;
        }
    }

}
