#include <ZxMem/ZxMem.h>
#include <format>
#include <cstdio>
#include <filesystem>


namespace ZQF
{
	ZxMem::ZxMem()
	{

	}

	ZxMem::~ZxMem()
	{

	}

	ZxMem::ZxMem(size_t nSize)
	{
		this->Resize(nSize, true);
	}

	ZxMem::ZxMem(const ZxMem& rfOBJ)
	{
		this->operator=(rfOBJ);
	}

	ZxMem::ZxMem(ZxMem&& rfOBJ) noexcept
	{
		this->operator=(std::move(rfOBJ));
	}

	ZxMem::ZxMem(const std::string_view msPath, size_t nLoadSize)
	{
		this->Load(msPath, nLoadSize);
	}

	auto ZxMem::operator=(const ZxMem& rfOBJ) -> ZxMem&
	{
		assert(this != &rfOBJ);

		m_nPos = rfOBJ.m_nPos;
		m_nSizeBytes = rfOBJ.m_nSizeBytes;
		if (rfOBJ.m_upMemData != nullptr)
		{
			m_upMemData = std::make_unique_for_overwrite<uint8_t[]>(rfOBJ.m_nSizeBytes);
			::memcpy(m_upMemData.get(), rfOBJ.m_upMemData.get(), m_nSizeBytes);
		}

		return *this;
	}

	auto ZxMem::operator=(ZxMem&& rfOBJ) noexcept -> ZxMem&
	{
		assert(this != &rfOBJ);

		m_nPos = rfOBJ.m_nPos;
		m_nSizeBytes = rfOBJ.m_nSizeBytes;
		m_upMemData = std::move(rfOBJ.m_upMemData);

		rfOBJ.m_nPos = 0;
		rfOBJ.m_nSizeBytes = 0;

		return *this;
	}

	auto ZxMem::Resize(size_t nNewSizeBytes, bool isDiscard) -> ZxMem&
	{
		if (m_upMemData == nullptr)
		{
			m_upMemData = std::make_unique_for_overwrite<uint8_t[]>(nNewSizeBytes);
		}
		else if (m_nSizeBytes < nNewSizeBytes)
		{
			auto tmp = std::make_unique_for_overwrite<uint8_t[]>(nNewSizeBytes);
			if (isDiscard == false)
			{
				::memcpy(tmp.get(), m_upMemData.get(), m_nSizeBytes);
			}
			m_upMemData = std::move(tmp);
		}

		m_nSizeBytes = nNewSizeBytes;
		return *this;
	}

	auto ZxMem::PosInc(size_t nBytes) -> ZxMem&
	{
		return this->PosSet<ZxMem::PosWay::Cur>(nBytes);
	}

	auto ZxMem::PosRewind() -> ZxMem&
	{
		return this->PosSet<ZxMem::PosWay::Beg>(0);
	}

	auto ZxMem::Save(const std::string_view msPath, bool isForceSave) const -> const ZxMem&
	{
		std::filesystem::path path = std::u8string_view{ reinterpret_cast<const char8_t*>(msPath.data()), msPath.size() };

		// force save
		if (isForceSave == false)
		{
			if (std::filesystem::is_regular_file(path))
			{
				throw std::runtime_error(std::format("ZxMem::Save(): file already exists!. msPath:{}", msPath));
			}
		}

		// make parent dir
		path.has_parent_path() ? (std::filesystem::create_directories(path.parent_path())) : (false);

		// create file
		FILE* fp{};
#ifdef _WIN32
		auto err = ::_wfopen_s(&fp, path.wstring().data(), L"wb");
		if ((err != 0) || (fp == nullptr)) { throw std::runtime_error(std::format("ZxMem::Save(): win32 create file error!. msPath: {}", msPath)); }
#else
		fp = std::fopen(msPath.data(), "wb");
		if (fp == nullptr) { throw std::runtime_error(std::format("ZxMem::Save(): linux create file error!. msPath: {}", msPath)); }
#endif

		// write data
		::fwrite(this->Ptr<uint8_t*>(), 1, this->SizeBytes(), fp);
		::fflush(fp);
		::fclose(fp);
		return *this;
	}

	auto ZxMem::Load(const std::string_view msPath, size_t nSize) -> ZxMem&
	{
		if (msPath.empty()) { throw std::runtime_error("ZxMem::Load(): path empty!"); }

		// open file
		FILE* fp{};
#ifdef _WIN32
		std::filesystem::path path = std::u8string_view{ reinterpret_cast<const char8_t*>(msPath.data()), msPath.size() };
		auto err = ::_wfopen_s(&fp, path.wstring().data(), L"rb");
		if ((err != 0) || (fp == nullptr)) { throw std::runtime_error(std::format("ZxMem::Load(): win32 open file error!. msPath: {}", msPath)); }
#else
		fp = std::fopen(msPath.data(), "rb");
		if (fp == nullptr) { throw std::runtime_error(std::format("ZxMem::Load(): linux open file error!. msPath: {}", msPath)); }
#endif

		// get file size
		size_t file_size{};
		::fseek(fp, 0, SEEK_END);
		file_size = ftell(fp);
		::fseek(fp, 0, SEEK_SET);

		// check read size
		size_t read_size_bytes{};
		if (file_size)
		{
			if (nSize == ZXMEM_AUTO_READ_SIZE)
			{
				read_size_bytes = static_cast<size_t>(file_size);
			}
			else
			{
				if (nSize <= file_size)
				{
					read_size_bytes = nSize;
				}
				else
				{
					throw std::runtime_error(std::format("ZxMem::Load(): read size larger than file size!, msPath: {}", msPath));
				}
			}
		}
		else
		{
			throw std::runtime_error(std::format("ZxMem::Load(): get file size error!, msPath: {}", msPath));
		}

		// read data
		::fread(this->Resize(read_size_bytes, true).Ptr<uint8_t*>(), 1, read_size_bytes, fp);

		// close file
		::fclose(fp);

		// rewind
		return this->PosRewind();
	}
}