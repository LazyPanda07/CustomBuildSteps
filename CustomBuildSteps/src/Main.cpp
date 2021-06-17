#include <iostream>
#include <filesystem>
#include <unordered_map>

using namespace std;

const unordered_map<string, string> extensions =
{
	{ ".h", "headers" },
	{ ".lib", "libs" },
	{ ".dll", "dlls" }
};
filesystem::path prefix;

void recursiveCopy(const filesystem::path& from, const filesystem::path& to);

int main(int argc, char** argv)
{
	if (argc != 6)
	{
		cout << "argc != 5" << endl;

		return -1;
	}

	filesystem::path binaries = filesystem::path(argv[1]).parent_path().parent_path();
	filesystem::path sources(argv[2]);
	filesystem::path to(argv[3]);
	filesystem::path docs(argv[4]);
	prefix = argv[5];

	filesystem::create_directories(to);

	for (const auto& [key, value] : extensions)
	{
		filesystem::create_directories(to / prefix / value);
	}

	for (const auto& i : filesystem::recursive_directory_iterator(binaries))
	{
		auto it = extensions.find(i.path().extension().string());

		if (it != extensions.end())
		{
			filesystem::copy(i, to / prefix / it->second, filesystem::copy_options::overwrite_existing);
		}
	}

	for (const auto& i : filesystem::directory_iterator(sources))
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

	if (filesystem::exists(docs) && !filesystem::exists(to / docs.filename()))
	{
		filesystem::create_directories(to / docs.filename());

		system(format("xcopy /E /I {} {}\\", docs.string(), (to / docs.filename()).string()).data());
	}

	return 0;
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
