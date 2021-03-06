
#include <gtest/gtest.h>

#include <Vector.h>
#include <ColumnWiseMatrix.h>

#include <WaveEquationSolver2D.h>
#include <IterableEnum.h>

namespace pdet
{
	class WaveEquation2DTests : public ::testing::Test
	{

	};

	TEST_F(WaveEquation2DTests, ConstantSolutionNoTransport)
	{
		// I chose double precision as implicit methods have a numerical error ~5e-5
		cl::dmat initialCondition(10, 8, 1.0f);
		cl::dvec xGrid = cl::LinSpace<MemorySpace::Device, MathDomain::Double>(0.0f, 1.0f, initialCondition.nRows());
		cl::dvec yGrid = cl::LinSpace<MemorySpace::Device, MathDomain::Double>(0.0f, 1.0f, initialCondition.nCols());
		double dt = 1e-5;
		float velocity = 0.0f;
		float diffusion = 0.0f;

		for (const SolverType solverType : enums::IterableEnum<SolverType>())
		{
			if (solverType != SolverType::ExplicitEuler && solverType != SolverType::ImplicitEuler)
				continue;

			pde::GpuDoublePdeInputData2D data(initialCondition, xGrid, yGrid, velocity, velocity, diffusion, dt, solverType, SpaceDiscretizerType::Centered);
			pde::dwave2D solver(data);

			const auto _initialCondition = solver.inputData.initialCondition.Get();
			for (unsigned n = 1; n < 10; ++n)
			{
				solver.Advance(10 * n);
				const auto solution = solver.solution->columns[0]->Get();

				for (size_t i = 0; i < solution.size(); ++i)
				{
					if (fabs(solution[i] - _initialCondition[i]) > 1e-5)
					{
						int a = 0;
					}
					ASSERT_TRUE(fabs(solution[i] - _initialCondition[i]) <= 1e-12);
				}
			}
		}
	}

	TEST_F(WaveEquation2DTests, ConstantSolution)
	{
		// I chose double precision as implicit methods have a numerical error ~5e-5
		cl::dmat initialCondition(10, 8, 1.0f);
		cl::dvec xGrid = cl::LinSpace<MemorySpace::Device, MathDomain::Double>(0.0f, 1.0f, initialCondition.nRows());
		cl::dvec yGrid = cl::LinSpace<MemorySpace::Device, MathDomain::Double>(0.0f, 1.0f, initialCondition.nCols());
		double dt = 1e-5;
		float xVelocity = .5f;
		float diffusion = 0.0f;

		for (const SolverType solverType : enums::IterableEnum<SolverType>())
		{
			if (solverType != SolverType::ExplicitEuler && solverType != SolverType::ImplicitEuler)
				continue;

			pde::GpuDoublePdeInputData2D data(initialCondition, xGrid, yGrid, xVelocity, 0.0, diffusion, dt, solverType, SpaceDiscretizerType::Centered);
			pde::dwave2D solver(data);

			const auto _initialCondition = solver.inputData.initialCondition.Get();
			for (unsigned n = 1; n < 10; ++n)
			{
				solver.Advance(10 * n);
				const auto solution = solver.solution->columns[0]->Get();

				for (size_t i = 0; i < solution.size(); ++i)
					ASSERT_TRUE(fabs(solution[i] - _initialCondition[i]) <= 1e-12);
			}
		}
	}

	TEST_F(WaveEquation2DTests, LinearSolution)
	{
		// I chose double precision as implicit methods have a numerical error ~5e-5
		cl::dvec xGrid = cl::LinSpace<MemorySpace::Device, MathDomain::Double>(0.0f, 1.0f, 10u);
		cl::dvec yGrid = cl::LinSpace<MemorySpace::Device, MathDomain::Double>(0.0f, 1.0f, 8u);
		double dt = 1e-5;
		float xVelocity = .1f;

		auto _xGrid = xGrid.Get();
		auto _yGrid = yGrid.Get();
		std::vector<double> _initialCondition(xGrid.size() * yGrid.size());
		for (unsigned j = 0; j < _yGrid.size(); ++j)
			for (unsigned i = 0; i < _xGrid.size(); ++i)
				_initialCondition[i + _xGrid.size() * j] = 2.0 * _xGrid[i] + 3.0 * _yGrid[j];

		cl::dmat initialCondition(_initialCondition, xGrid.size(), yGrid.size());

		for (const SolverType solverType : enums::IterableEnum<SolverType>())
		{
			if (solverType != SolverType::ExplicitEuler && solverType != SolverType::ImplicitEuler)
				continue;

			// need to setup the correct boundary condition with the slope of the planes
			BoundaryCondition leftBoundaryCondition(BoundaryConditionType::Neumann, 3.0);
			BoundaryCondition rightBoundaryCondition(BoundaryConditionType::Neumann, -3.0);
			BoundaryCondition downBoundaryCondition(BoundaryConditionType::Neumann, -2.0);
			BoundaryCondition upBoundaryCondition(BoundaryConditionType::Neumann, 2.0);
			BoundaryCondition2D boundaryConditions(leftBoundaryCondition, rightBoundaryCondition,
												   downBoundaryCondition, upBoundaryCondition);

			pde::GpuDoublePdeInputData2D data(initialCondition, xGrid, yGrid, xVelocity, 0.0, 0.0, dt, solverType, SpaceDiscretizerType::Centered, boundaryConditions);
			pde::dwave2D solver(data);

			const auto _initialCondition = solver.inputData.initialCondition.Get();
			for (unsigned n = 1; n < 10; ++n)
			{
				solver.Advance(n);
				const auto solution = solver.solution->columns[0]->Get();

				// excluding corners
				for (unsigned j = 1; j < _yGrid.size() - 1; ++j)
				{
					for (unsigned i = 1; i < _xGrid.size() - 1; ++i)
					{
						const unsigned idx = i + _xGrid.size() * j;
						ASSERT_LE(fabs(solution[idx] - _initialCondition[idx]), 5e-12);
					}
				}
			}
		}
	}
}