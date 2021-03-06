
#pragma once

#include "RedstoneHandler.h"
#include "BlockEntities/ChestEntity.h"





class cTrappedChestHandler : public cRedstoneHandler
{
	typedef cRedstoneHandler super;
public:

	cTrappedChestHandler(cWorld & a_World) :
		super(a_World)
	{
	}

	virtual unsigned char GetPowerDeliveredToPosition(const Vector3i & a_Position, BLOCKTYPE a_BlockType, NIBBLETYPE a_Meta, const Vector3i & a_QueryPosition, BLOCKTYPE a_QueryBlockType) override
	{
		UNUSED(a_BlockType);
		UNUSED(a_Meta);
		UNUSED(a_QueryPosition);
		UNUSED(a_QueryBlockType);

		return static_cast<cIncrementalRedstoneSimulator *>(m_World.GetRedstoneSimulator())->GetChunkData()->GetCachedPowerData(a_Position).PowerLevel;
	}

	virtual unsigned char GetPowerLevel(const Vector3i & a_Position, BLOCKTYPE a_BlockType, NIBBLETYPE a_Meta) override
	{
		UNUSED(a_BlockType);
		UNUSED(a_Meta);

		class cGetTrappedChestPlayers :
			public cItemCallback<cChestEntity>
		{
		public:
			cGetTrappedChestPlayers(void) :
				m_NumberOfPlayers(0)
			{
			}

			virtual ~cGetTrappedChestPlayers()
			{
			}

			virtual bool Item(cChestEntity * a_Chest) override
			{
				ASSERT(a_Chest->GetBlockType() == E_BLOCK_TRAPPED_CHEST);
				m_NumberOfPlayers = a_Chest->GetNumberOfPlayers();
				return true;
			}

			unsigned char GetPowerLevel(void) const
			{
				return static_cast<unsigned char>(std::min(m_NumberOfPlayers, 15));
			}

		private:
			int m_NumberOfPlayers;

		} GTCP;

		VERIFY(!m_World.DoWithChestAt(a_Position.x, a_Position.y, a_Position.z, GTCP));
		return GTCP.GetPowerLevel();
	}

	virtual cVector3iArray Update(const Vector3i & a_Position, BLOCKTYPE a_BlockType, NIBBLETYPE a_Meta, PoweringData a_PoweringData) override
	{
		// LOGD("Evaluating tricky the trapped chest (%d %d %d)", a_Position.x, a_Position.y, a_Position.z);

		auto Power = GetPowerLevel(a_Position, a_BlockType, a_Meta);
		auto PreviousPower = static_cast<cIncrementalRedstoneSimulator *>(m_World.GetRedstoneSimulator())->GetChunkData()->ExchangeUpdateOncePowerData(a_Position, PoweringData(a_BlockType, Power));

		if (Power != PreviousPower.PowerLevel)
		{
			return GetAdjustedRelatives(a_Position, StaticAppend(GetRelativeLaterals(), cVector3iArray{ OffsetYM() }));
		}

		return {};
	}

	virtual cVector3iArray GetValidSourcePositions(const Vector3i & a_Position, BLOCKTYPE a_BlockType, NIBBLETYPE a_Meta) override
	{
		UNUSED(a_Position);
		UNUSED(a_BlockType);
		UNUSED(a_Meta);

		return {};
	}
};
