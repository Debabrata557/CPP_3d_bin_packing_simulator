import sys
import argparse
import os
import json
import time
from dask.config import get
import numpy as np
from termcolor import colored
from es import CMAES, PEPG, OpenES
from distributed import Client, progress
import subprocess
import pickle
np.random.seed(0)

def get_args():
    parser = argparse.ArgumentParser()
    
    parser.add_argument('--test', action='store_true', help='Test the experiment')
    parser.add_argument('--load',  help='Experiment to be loaded',default=None)
    parser.add_argument('--sol', type=int, help='Solution: 1:PS 2:PS with delay 3: PS with split state',default=1)
    parser.add_argument('--train-based', action='store_true', help='Scheduler is train based')    
    parser.add_argument('--policy-threshold', type=float, help='Policy threshold for train based scheduler', default=0.0)    
    parser.add_argument('--perturb-value', type=int, help='Max perturbation value', default=30)
    parser.add_argument('--iterations', type=int, help='Max perturbation value', default=4000)
    parser.add_argument('--preprocess', action='store_true',help='Use greedy strategy to move train')
    parser.add_argument('--stochastic', action='store_true',help='Make move decisions stocastically')
    parser.add_argument('-p', '--population-size', type=int, help='Population size', default=50)
    parser.add_argument('-b', '--lb', type=int,help='Number of resources in the back', default=2)
    parser.add_argument('-f', '--lf', type=int,help='Number of resources in the front', default=6)
    parser.add_argument('--num-simulations', type=int,help='Number of simulations to average over', default=10)
    parser.add_argument('--num-parameters', type=int,help='Number of parameters', default=352)
    parser.add_argument('--hidden-size', type=int,help='Number of nodes in hidden layer', default=10)
    parser.add_argument('--n-layers', type=int,help='Number of hidden layers', default=3)
    parser.add_argument('-e', '--experiment',help='Experiment name')
    parser.add_argument('-N', '--network',help='Network name',default="HYP-1")
    parser.add_argument('-s', '--scheduler',help='Scheduler IP with port', default=None)
    parser.add_argument('--save-after',type=int,help='Save solver after every n generations', default=1000)
    return vars(parser.parse_args())

def pretty_format_row(row, header=False):
    import subprocess, functools
    height, width = map(int, subprocess.check_output(['stty', 'size']).decode().split())
    pad = int((width - len(row) - 1) / len(row))
    return '|'.join([pretty_format(row[i], i, pad, header) for i in range(len(row))])

def pretty_format(d, i, pad, header=False):
    colors = ['red', 'green', 'yellow', 'blue', 'magenta', 'cyan']
    i = i % len(colors)
    d = d[:pad]
    pad = pad - len(d)
    d = ' ' * int(pad/2) + d + ' ' * (pad - int(pad/2))
    # print(d, len(d))
    return colored(d, colors[i]) if not header else colored(d, colors[i], attrs=['reverse'])


def run_subproccess(args):
    t1 = time.time()
    s = subprocess.call(args)
    t2 = time.time()
    with open(args[2]) as f:
        eff = float(f.readline())
        bp = float(f.readline())
    return {"eff":eff, "boxes_put":bp,"genTime":(t2-t1)*1000}



if __name__ == '__main__':
    
    # args = get_args()

    # NUM_SIMULATIONS = args['num_simulations']
    client = Client()
    FILTER_SIZE = 20
    STRIDE = 20
    EXTRACT_FEATURE_AREA=70
    BIN_WIDTH=120
    BIN_LENGTH=180
    # NPARAMS_X = (120+STRIDE-1)//STRIDE
    # NPARAMS_Y = (180+STRIDE-1)//STRIDE
    NPARAMS_X = ((BIN_WIDTH//2) + STRIDE - 1) // STRIDE
    NPARAMS_Y = ((BIN_LENGTH//2) + STRIDE - 1) // STRIDE
    # NPARAMS_X = (EXTRACT_FEATURE_AREA+STRIDE-1)//STRIDE
    # NPARAMS_Y = (EXTRACT_FEATURE_AREA+STRIDE-1)//STRIDE
    NPARAMS = 3*NPARAMS_X*NPARAMS_Y+1+1+1
    # print('Number of parameters:', NPARAMS)
    directory = "test_dir"
    NPOPULATION = 25
    MAXITER = 100000


    def save_weights_to_file(weights, filename):
         with open(filename,"w") as f:
            for w in weights:
                f.write(f'{w}\n')
    
    # def print_history_to_file(history):
    #     with open(f'{directory}/history.csv','a') as file:
    #         for r in history:
    #             s = ",".join(map(str, r))
    #             file.write(f'{s}\n')

    
    def run_overfit(solver):
        # sigmas=[]
        # best_result_obj1 = float('inf')
        # best_result_cur_obj = float('inf')
        for j in range(MAXITER):
            tic1 = time.time()
            solutions = solver.ask()

            fitness_list_futures = []
            seed = 75
            #seed=0
            fitness_list_futures.append(
                            client.map(
                                fit_func,
                                [seed for x in range(NPOPULATION)],
                                [1 for x in range(NPOPULATION)],
                                ["smart_algo" for x in range(NPOPULATION)],
                                solutions,
                                [x for x in range(NPOPULATION)],
                                pure=False
                            )
                        )

            progress(fitness_list_futures)
            results = client.gather(fitness_list_futures)
            del fitness_list_futures
            tac1 = time.time()
            effs = [r["eff"] for r in results[0]]
            boxes_put = [r["boxes_put"] for r in results[0]]
            genTime = [r["genTime"] for r in results[0]]

            solver.tell([x for x in effs])
            if solver.es.sigma>100:
                break

            row = [
					str(j+1),
					str(np.min(effs)),
					str(np.mean(effs)),
					str(np.max(effs)),
					str(solver.es.sigma),
					str(np.min(boxes_put)),
					str(np.mean(boxes_put)),
					str(np.max(boxes_put)),
                    str(np.mean(genTime)),
					str((tac1-tic1)*1000)
				]

            # _row = pretty_format_row(row)
            print('train result: ')
            print(row)

    def run_solver(solver):
        # sigmas=[]
        # best_result_obj1 = float('inf')
        # best_result_cur_obj = float('inf')
        file = open('test_results.txt','w')
        for j in range(MAXITER):
            tic1 = time.time()
            solutions = solver.ask()

            fitness_list_futures = []
            seed = np.random.randint(0,1000)
            #seed=0
            fitness_list_futures.append(
                            client.map(
                                fit_func,
                                [seed for x in range(NPOPULATION)],
                                [1 for x in range(NPOPULATION)],
                                ["smart_algo" for x in range(NPOPULATION)],
                                solutions,
                                [x for x in range(NPOPULATION)],
                                pure=False
                            )
                        )

            progress(fitness_list_futures)
            results = client.gather(fitness_list_futures)
            del fitness_list_futures
            tac1 = time.time()
            effs = [r["eff"] for r in results[0]]
            boxes_put = [r["boxes_put"] for r in results[0]]
            genTime = [r["genTime"] for r in results[0]]

            solver.tell([x for x in effs])
            if solver.es.sigma>100:
                break

            row = [
					str(j+1),
					str(np.min(effs)),
					str(np.mean(effs)),
					str(np.max(effs)),
					str(solver.es.sigma),
					str(np.min(boxes_put)),
					str(np.mean(boxes_put)),
					str(np.max(boxes_put)),
                    str(np.mean(genTime)),
					str((tac1-tic1)*1000)
				]

            # _row = pretty_format_row(row)
            print('train result: ')
            print(row)
            run_test(solver, file)
        file.close()
            # history.append(row)

            # if j%100 == 0:
                # print_history_to_file(history)
                # history.clear()

            # if j%SAVE_AFTER == 0 and j>0:
                # save_weights_to_file(solver.current_param().tolist(),f'{directory}/mean_weights{j}.txt')
                # save_weights_to_file(solver.best_param().tolist(),f'{directory}/best_weights{j}.txt')
                # with open(f'{directory}/solver{j}', 'wb') as solver_file:
                    # pickle.dump(solver, solver_file)
                # run_test(str(j),str(j))
                
        # print_history_to_file(history)
        # save_weights_to_file(solver.current_param().tolist(),f'{directory}/mean_weights.txt')
        # save_weights_to_file(solver.best_param().tolist(),f'{directory}/best_weights.txt')

        # with open(f'{directory}/solver', 'wb') as solver_file:
                # pickle.dump(solver, solver_file)
    def save_weights_to_file(weights, filename):
         with open(filename,"w") as f:
            for w in weights:
                f.write(f'{w}\n')

    def run_test(solver, file):
        test_episode=30
        tic1 = time.time()
        bestFile='bestWeights.txt'
        bestMean=-1

        fitness_list_futures = []
        seed = np.random.randint(0,1000)
        #seed=0
        fitness_list_futures.append(
                    client.map(
                        fit_func,
                        [seed+x for x in range(test_episode)],
                        [1 for x in range(test_episode)],
                        ["smart_algo" for x in range(test_episode)],
                        [solver.current_param() for x in range(test_episode)],
                        [x for x in range(test_episode)],
                        pure=False
                    )
                )

        progress(fitness_list_futures)
        results = client.gather(fitness_list_futures)
        del fitness_list_futures
        tac1 = time.time()
        effs = [r["eff"] for r in results[0]]
        boxes_put = [r["boxes_put"] for r in results[0]]
        genTime = [r["genTime"] for r in results[0]]
        if np.mean(effs)>bestMean:
                save_weights_to_file(solver.current_param().tolist(),f'{directory}/{bestFile}')
        row = [
                str(np.min(effs)),
                str(np.mean(effs)),
                str(np.max(effs)),
                str(solver.es.sigma),
                str(np.min(boxes_put)),
                str(np.mean(boxes_put)),
                str(np.max(boxes_put)),
                str(np.mean(genTime)),
                str((tac1-tic1)*1000)
            ]


        file.write(str(np.mean(effs))+'\n')
        # _row = pretty_format_row(row)
        print('test results: ')
        print(row)

    def run_simulation(seed,episode,algo_name, weights=None, gen=None):
        filename_in = f'{directory}/weights_{gen}.txt'
        filename_out = f'{directory}/result_{gen}.txt'
        save_weights_to_file(weights,filename_in)
        debug=0
        return run_subproccess(['./build/run',filename_in,filename_out,str(seed),str(episode), algo_name, str(debug)])


    # if args['test']:
    #     print("Testing...")
    #     run_test("","_test")
    #     sys.exit()

    if not os.path.exists(directory):
        os.makedirs(directory)
    # else:
    #     print(colored('Experiement exists!', 'red'))
        # sys.exit()

    fit_func = run_simulation
    # history = []
    weights = None

    # if args['load'] != None:
    #     exp_load = args['load']
    #     exp_load_directory = f'experiments/{exp_load}'
    #     if os.path.exists(f'{exp_load_directory}/solver'):
    #         with open(f'{exp_load_directory}/solver', 'rb') as file:
    #             solver = pickle.load(file)
    #         with open(f'{exp_load_directory}/parameters.json', 'r') as file:
    #             load_args = json.load(file)
    #         load_args['iterations'] = args['iterations']
    #         load_args['load'] = args['load']
    #         load_args['experiment'] = args['experiment']
    #         args = load_args
    #     else:
    #         print(colored('Experiement to load does not exists!', 'red'))
    #         sys.exit()
    # else:
    solver = CMAES(
            NPARAMS,
            popsize=NPOPULATION,
            weight_decay=0.01,
            sigma_init=0.5,
            initial_weights=weights
        )

    # with open(f'{directory}/parameters.json', 'w') as parameters_file:
    #     json.dump(args, parameters_file)

    run_solver(solver)
    #run_overfit(solver)
    #run_simulation(0,30,"smart_algo", weights=[0]*46, gen=0)

    # for i in range(NPOPULATION):
    #     os.remove(f'{directoryweights}/weights_{i}.txt')
    #     os.remove(f'{directory}/result_{i}.txt')

    # run_test("")








