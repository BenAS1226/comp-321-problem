import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.StringTokenizer;

public class Main {
	// Program ends after 300 offers
	static final int MAX_OFFERS = 300;

	static Map<String, Integer> NAME_TO_IDX = new HashMap<>();
	static Map<Integer, String> IDX_TO_NAME = new HashMap<>();

	static class Bundle implements Comparable<Bundle> {
		int cost;
		int totalPieces;
		int[] counts;

		Bundle(int cost, int totalPieces, int[] counts) {
			this.cost = cost;
			this.totalPieces = totalPieces;
			this.counts = counts;
		}

		@Override
		public int compareTo(Bundle other) {
			if (this.cost != other.cost) {
				return Integer.compare(this.cost, other.cost);
			}
			return Integer.compare(this.totalPieces, other.totalPieces);
		}
	}

	// All possible combos of candy under 10 pieces
	static List<Bundle> generateAllBundles(int[] aVals, int maxPieces) {
		List<Bundle> bundles = new ArrayList<>();
		for (int c0 = 0; c0 <= maxPieces; c0++) {
			for (int c1 = 0; c1 <= maxPieces - c0; c1++) {
				for (int c2 = 0; c2 <= maxPieces - c0 - c1; c2++) {
					for (int c3 = 0; c3 <= maxPieces - c0 - c1 - c2; c3++) {
						for (int c4 = 0; c4 <= maxPieces - c0 - c1 - c2 - c3; c4++) {
							int totalPieces = c0 + c1 + c2 + c3 + c4;
							if (totalPieces >= 1 && totalPieces <= maxPieces) {
								int[] counts = new int[] {c0, c1, c2, c3, c4};
								int cost = c0 * aVals[0] + c1 * aVals[1] + c2 * aVals[2] + c3 * aVals[3] + c4 * aVals[4];
								bundles.add(new Bundle(cost, totalPieces, counts));
							}
						}
					}
				}
			}
		}
		Collections.sort(bundles);
		return bundles;
	}

	// All possible b-value vectors (Bob's satisfaction) in [1..maxB]^5
	static List<int[]> initCandidates(int maxB) {
		List<int[]> candidates = new ArrayList<>();
		for (int b0 = 1; b0 <= maxB; b0++) {
			for (int b1 = 1; b1 <= maxB; b1++) {
				for (int b2 = 1; b2 <= maxB; b2++) {
					for (int b3 = 1; b3 <= maxB; b3++) {
						for (int b4 = 1; b4 <= maxB; b4++) {
							candidates.add(new int[] {b0, b1, b2, b3, b4});
						}
					}
				}
			}
		}
		return candidates;
	}

	// Bob accepts if gain is more than 0
	static List<int[]> filterCandidates(List<int[]> candidates, int[] offer, int listType, int listCnt, boolean accepted) {
		List<int[]> filtered = new ArrayList<>();
		for (int[] b : candidates) {
			int gain = 0;
			for (int i = 0; i < 5; i++) {
				gain += offer[i] * b[i];
			}
			gain -= listCnt * b[listType];
			if ((gain > 0) == accepted) {
				filtered.add(b);
			}
		}
		return filtered.isEmpty() ? candidates : filtered;
	}

	// Selects profitable bundle with the highest expected value given remaining
	static int[] chooseBestBundle(
	    List<Bundle> allBundles, List<int[]> candidates, int[] avail,
	    int[] baseOffer, int baseCost, int basePieces,
	    int listType, int listCnt, int listVal) {

		int[] bestBundle = null;
		double bestScore = -1.0;
		int nCand = candidates.size();
		int[] totOffer = new int[5];

		for (Bundle bObj : allBundles) {
			if (basePieces + bObj.totalPieces > 10) {
				continue;
			}

			int[] bundle = bObj.counts;
			boolean possible = true;
			for (int i = 0; i < 5; i++) {
				if (bundle[i] > avail[i]) {
					possible = false;
					break;
				}
			}
			if (!possible) {
				continue;
			}

			int totalCost = baseCost + bObj.cost;
			int profit = listVal - totalCost;
			// Weeds out bad bundles
			if (profit <= 0) {
				continue;
			}

			for (int i = 0; i < 5; i++) {
				totOffer[i] = baseOffer[i] + bundle[i];
			}

			int acceptCnt = 0;
			for (int[] b : candidates) {
				int gain = totOffer[0] * b[0] + totOffer[1] * b[1] + totOffer[2] * b[2]
				           + totOffer[3] * b[3] + totOffer[4] * b[4] - listCnt * b[listType];
				if (gain > 0) {
					acceptCnt++;
				}
			}

			if (acceptCnt == 0) {
				continue;
			}

			double prob = (double) acceptCnt / nCand;
			double score = prob * profit;

			if (score > bestScore) {
				// Updates best bundle
				bestScore = score;
				bestBundle = bundle;
			}
		}
		return bestBundle;
	}

	// If no profitable offer exists, picks 1 piece of cheapest candy instead
	static int[] getFallbackBundle(int[] avail, int[] aVals) {
		int bestIdx = -1;
		int lowestVal = Integer.MAX_VALUE;
		for (int i = 0; i < 5; i++) {
			if (avail[i] > 0 && aVals[i] < lowestVal) {
				lowestVal = aVals[i];
				bestIdx = i;
			}
		}
		if (bestIdx == -1) {
			return null;
		}
		int[] bundle = new int[5];
		bundle[bestIdx] = 1;
		return bundle;
	}

	// Function to format offer aka OFFER candy amount
	static String formatOffer(int[] offer) {
		StringBuilder sb = new StringBuilder("OFFER");
		for (int i = 0; i < 5; i++) {
			if (offer[i] > 0) {
				sb.append(" ").append(IDX_TO_NAME.get(i)).append(" ").append(offer[i]);
			}
		}
		return sb.toString();
	}

	static int sumArray(int[] arr) {
		int s = 0;
		for (int v : arr) s += v;
		return s;
	}

	public static void main(String[] args) throws IOException {
		BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
		String line = reader.readLine();
		if (line == null || line.trim().isEmpty()) {
			return;
		}

		// Satisfaction
		int S = Integer.parseInt(line.trim());

		int[] owned = new int[5];
		int[] aVals = new int[5];

		// To keep track of owned and available candy
		for (int idx = 0; idx < 5; idx++) {
			StringTokenizer st = new StringTokenizer(reader.readLine());
			String name = st.nextToken();
			int o = Integer.parseInt(st.nextToken());
			int a = Integer.parseInt(st.nextToken());
			NAME_TO_IDX.put(name, idx);
			IDX_TO_NAME.put(idx, name);
			owned[idx] = o;
			aVals[idx] = a;
		}

		int N = Integer.parseInt(reader.readLine().trim());
		int[][] listings = new int[N][2];
		for (int i = 0; i < N; i++) {
			StringTokenizer st = new StringTokenizer(reader.readLine());
			String name = st.nextToken();
			int c = Integer.parseInt(st.nextToken());
			listings[i][0] = NAME_TO_IDX.get(name);
			listings[i][1] = c;
		}

		// Using helper functions
		List<Bundle> allBundles = generateAllBundles(aVals, 10);
		List<int[]> candidates = initCandidates(10);

		// Setting up bowl and offers
		int finalBowl = 0;
		int offersMade = 0;

		for (int step = 0; step < N; step++) {
			if (sumArray(owned) == 0) {
				break; // Cannot make any valid non-empty offer if inventory is completely empty
			}

			int listType = listings[step][0];
			int listCnt = listings[step][1];
			int listVal = listCnt * aVals[listType];

			// Bob's first offer
			int[] offer1 = chooseBestBundle(
			                   allBundles, candidates, owned, new int[] {0, 0, 0, 0, 0}, 0, 0, listType, listCnt, listVal
			               );

			if (offer1 == null) {
				// Use worst-case scenario function
				offer1 = getFallbackBundle(owned, aVals);
			}

			// Print Bob's first offer and flush output for interactive judge
			System.out.println(formatOffer(offer1));
			System.out.flush();
			offersMade++;

			// Documenting user response
			String resp = reader.readLine();
			if (resp == null || resp.trim().isEmpty()) {
				break;
			}
			resp = resp.trim();

			// If user accepts offer
			if (resp.equals("ACCEPT")) {
				candidates = filterCandidates(candidates, offer1, listType, listCnt, true);
				for (int i = 0; i < 5; i++) {
					owned[i] -= offer1[i];
				}
				finalBowl += listVal;

				// If user wants more candy from the offer
			} else if (resp.equals("ADD")) {
				candidates = filterCandidates(candidates, offer1, listType, listCnt, false);
				int[] remAvail = new int[5];
				int baseCost = 0;
				for (int i = 0; i < 5; i++) {
					remAvail[i] = owned[i] - offer1[i];
					baseCost += offer1[i] * aVals[i];
				}
				int basePieces = sumArray(offer1);

				int[] offer2 = chooseBestBundle(allBundles, candidates, remAvail, offer1, baseCost, basePieces, listType, listCnt, listVal
				                               );
				// Minimal 1-candy fallback offer
				if (offer2 == null && basePieces < 10) {
					offer2 = getFallbackBundle(remAvail, aVals);
				}
				if (offer2 != null) {
					System.out.println(formatOffer(offer2));
					System.out.flush();
					offersMade++;
					String resp2 = reader.readLine();
					if (resp2 == null || resp2.trim().isEmpty()) {
						break;
					}
					resp2 = resp2.trim();
					int[] totOffer = new int[5];
					for (int i = 0; i < 5; i++) {
						totOffer[i] = offer1[i] + offer2[i];
					}
					if (resp2.equals("ACCEPT")) {
						candidates = filterCandidates(candidates, totOffer, listType, listCnt, true);
						for (int i = 0; i < 5; i++) {
							owned[i] -= totOffer[i];
						}
						finalBowl += listVal;
					} else {
						// If response is decline then loop runs again
						candidates = filterCandidates(candidates, totOffer, listType, listCnt, false);
					}
				}
			}
		}
	}
}
