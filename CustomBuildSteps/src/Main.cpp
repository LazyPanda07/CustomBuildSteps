#include <iostream>
#include <filesystem>
#include <unordered_map>

using namespace std;

const unordered_map<string, string> extensions =
{
	{ ".h", "headers" },
	{ ".lib", "libs" },
	{ ".dll", "dlls" },
	{ ".pdb", "pdbs" },
	{ ".idb", "idbs" }
};
filesystem::path prefix;

void copyHeaders(const filesystem::path& pathToHeaders, const filesystem::path& to);

void recursiveCopy(const filesystem::path& from, const filesystem::path& to);

int main(int argc, char** argv)
{
	if (argc < 6)
	{
		cout << "argc must be >= 6" << endl;

		return -1;
	}

	filesystem::path binaries = filesystem::path(argv[1]).parent_path().parent_path();
	filesystem::path sources(argv[2]);
	filesystem::path to(argv[3]);
	filesystem::path docs(argv[4]);
	prefix = argv[5];

	vector<filesystem::path> submodules;

	for (size_t i = 6; i < argc; i++)
	{
		submodules.push_back(argv[i]);
	}

	filesystem::create_directories(to);

	for (const auto& [key, value] : extensions)
	{
		filesystem::create_directories(to / prefix / value);
	}

	for (const auto& i : filesystem::recursive_directory_iterator(binaries))
	{
		if (i.path().string().find("Test") != string::npos)
		{
			continue;
		}

		auto it = extensions.find(i.path().extension().string());

		if (it != extensions.end())
		{
			filesystem::copy(i, to / prefix / it->second, filesystem::copy_options::overwrite_existing);
		}
	}

	copyHeaders(sources, to);

	for (const auto& i : submodules)
	{
		copyHeaders(i, to);
	}

	if (filesystem::exists(docs) && !filesystem::exists(to / docs.filename()))
	{
		filesystem::create_directories(to / docs.filename());

		system(format("xcopy /E /I {} {}\\", docs.string(), (to / docs.filename()).string()).data());
	}

	for (const auto& [key, value] : extensions)
	{
		if (filesystem::is_empty(to / prefix / value))
		{
			filesystem::remove(to / prefix / value);
		}
	}

	return 0;
}

void copyHeaders(const filesystem::path& pathToHeaders, const filesystem::path& to)
{
	for (const auto& i : filesystem::directory_iterator(pathToHeaders))
	{
		if (filesystem::is_directory(i))
		{
			filesystem::path folder = to / prefix / "headers" / i.path().filename();

			filesystem::create_directories(folder);

			recursiveCopy(i, folder);
		}
		else
		{
			auto it = extensions.find(i.path().extension().string());

			if (it != extensions.end())
			{
				filesystem::copy(i, to / prefix / it->second, filesystem::copy_options::overwrite_existing);
			}
		}
	}
}

void recursiveCopy(const filesystem::path& from, const filesystem::path& to)
{
	for (const auto& i : filesystem::directory_iterator(from))
	{
		if (filesystem::is_directory(i))
		{
			filesystem::path folder = to / i.path().filename();

			filesystem::create_directories(folder);

			recursiveCopy(from / i, to / folder);
		}
		else
		{
			auto it = extensions.find(i.path().extension().string());

			if (it != extensions.end())
			{
				filesystem::copy(i, to, filesystem::copy_options::overwrite_existing);
			}
		}
	}
}
