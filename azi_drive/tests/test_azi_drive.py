#!/usr/bin/env python
PKG='test_azi_drive'
import sys
import unittest

from azimuth_drive import Azi_Drive
from azimuth_drive import Tools
from time import time
import numpy as np


class Test_Azi_Drive(unittest.TestCase):
    '''Unit tests for the Azi Drive package
    TODO:
        - Add convexity tests for the objective function
            -- Requires restructuring of the way objective(*) is defined and called
            -- Is the best way to test that an objective function will work

        - Add tests for making the thrust zeroed when zero force is asked

    '''

    def setUp(self):
        self.maxDiff = None
        self.test_time = False

    def test_thrust_matrix(self):
        '''Test the thrust matrix
        i.e.: Prove that the thrust matrix generated by azi_drive accurately represents the 
            dynamics of the boat

        TODO
            This needs more real test cases, but I don't really want to do the math
        '''

        tests = [
            {
                'angles': (0.0, 0.0),
                'u': (10, 10),
                'answer': [20, 0, 0],
            },
            {
                'angles': (0.0, 0.0),
                'u': (-10, -10),
                'answer': [-20, 0, 0],
            },
            {
                'angles': (np.pi / 2, -np.pi / 2),
                'u': (10, 10),
                'answer': [0.0, 0.0, 0.0],
            },
            {
                'angles': (np.pi, np.pi),
                'u': (10, 10),
                'answer': [-20.0, 0.0, 0.0],
            },
            {
                'angles': (0.0, 0.0),
                'u': (10, -10),
                'answer': [0.0, 0.0, -6.0],
            },
            {
                'angles': (0.0, 0.0),
                'u': (10, -10),
                'answer': [0.0, 0.0, -6.0],
            },
            # {
            #     'angles': (np.pi / 2, 0.0),
            #     'u': (10, 0.0),
            #     'answer': [0.0, 0.0, -6.0],
            # }

        ]
        for test in tests:
            B = Azi_Drive.thrust_matrix(test['angles'])
            u = np.array(test['u'])
            net = np.dot(B, u)
            self.assertTrue(np.allclose(net, test['answer']), 
                msg='Expecting net of {}, got {}, for inputs of u={}, alpha={}'.format(
                    test['answer'], 
                    net, 
                    test['u'], 
                    test['angles'],
                )
            )

    @unittest.skip("Redundant - test_thrust_matrix does this")
    def test_net_force(self):
        net = Azi_Drive.net_force((0.0, 0.0), (10.0, 10.0))
        self.assertTrue(np.allclose(net.T, [-20, 0, 0]))

    def test_singularity(self):
        cost = Azi_Drive.singularity_avoidance((0.0, 0.0))

    def test_jacobian_test_1(self):
        def test_1((x, y)):
            return (x ** 2) + (y ** 2)

        jacobian = Tools.jacobian(test_1, np.array([1.0, 1.0]))
        self.assertTrue(np.allclose(jacobian, [2.0, 2.0]))

    def test_jacobian_test_2(self):
        def test_2((x, y)):
            return np.array([(x ** 2) + (y ** 3), 3 * y])

        exp_result = np.matrix([
            [2, 3],
            [0, 3],
        ])

        jacobian = Tools.jacobian(test_2, np.array([1.0, 1.0]))
        self.assertTrue(np.allclose(jacobian, exp_result))

    def test_jacobian_singularity_avoidance(self):
        '''Not such a good test, very hard to analytically get this jacobian
        Really, just proving that Tools.jacobian doesn't crash
        '''
        jacobian = Tools.jacobian(Azi_Drive.singularity_avoidance, np.array([1.0, 1.0]), dx=0.1)
        self.assertEqual(jacobian.shape, (1, 2))

    def test_thrust_jacobian(self):
        '''Test thrust_jacobian

        i.e. test that 
            jac(thrust_matrix(alpha) * u) * delta_alpha 
                + thrust_matrix(alpha_0) * u_0
            [jacobian evaluated at alpha = alpha_0, u = u_0]

        is close to thrust_matrix(delta_alpha + alpha_0)

        In English: That the implemented jacobian suffices to do a higher dimensional
            first-order Taylor approximation of the nonlinear behavior of the thrust_matrix

        '''
        max_error = 0.2

        tests = [
            {
                'u_0': (10, 10),
                'alpha_0': (0.01, 0.01),
                'delta_angle': (0.1, 0.1),
            },
            {
                'u_0': (10, 10),
                'alpha_0': (0.1, 0.1),
                'delta_angle': (0.05, 0.05),
            },
            {
                'u_0': (25, 10),
                'alpha_0': (0.5, 0.3),
                'delta_angle': (0.08, 0.05),
            },
        ]

        for test in tests:
            u_0 = np.array(test['u_0'])
            alpha_0 = np.array(test['alpha_0'])
            delta_angle = np.array(test['delta_angle'])

            jacobian = Azi_Drive.thrust_jacobian(alpha_0, u_0)

            initial_thrust_matrix = Azi_Drive.thrust_matrix(alpha_0)
            linear_estimate = np.dot(initial_thrust_matrix, u_0) + np.dot(jacobian, delta_angle)

            true_thrust_matrix = Azi_Drive.thrust_matrix(alpha_0 + delta_angle)
            nonlinear_estimate = np.dot(true_thrust_matrix, u_0)

            error = np.linalg.norm(nonlinear_estimate - linear_estimate)

            self.assertLess(
                error, 
                max_error, 
                msg='error of {} exceeds maximum of {}\n Estimates: nonlinear {}, linear {}'.format(
                    error,
                    max_error,
                    nonlinear_estimate, 
                    linear_estimate, 
                )
            )

    def test_allocation(self):
        max_time = 0.06
        max_error = 15 # A little big, but hey
        def run_test(fx, fy, moment):
            u_nought = np.array([0.0, 0.0])
            alpha_nought = np.array([0.1, 0.1])

            tau = np.array([fx, fy, moment])

            #  Accumulate 20 iterations
            for k in range(20):
                tic = time()
                thrust_solution = Azi_Drive.map_thruster(
                    fx_des=fx, 
                    fy_des=fy, 
                    m_des=moment, 
                    alpha_0=alpha_nought, 
                    u_0=u_nought
                )
                toc = time() - tic

                d_theta, d_force = thrust_solution
                alpha_nought += d_theta
                u_nought += d_force

                if self.test_time:
                    self.assertLess(toc, max_time, msg="Executon took more then {} sec".format(toc))

            net = Azi_Drive.net_force(alpha_nought, u_nought)
            difference = np.linalg.norm(net - tau)
            success = difference < 0.1

            self.assertLess(
                difference, 
                max_error,
                msg="Failed on request {}, with error {}, producing {}".format(
                    (fx, fy, moment), 
                    difference,
                    net
                )
            )
            return success

        tests = [
            (0, 0, 0),
            (20, 70, -3),
            (100, 0, 0),
            (0, 0, 5),
            (180, 0, 0),
            (200, 0, 0),
            (100, 20, -3),
            (120, 15, 12),
        ]
        results = {}
        for test in tests:
            result = run_test(*test)


if __name__ == '__main__':
    import rosunit
    rosunit.unitrun(PKG, 'test_azi_drive', Test_Azi_Drive)
