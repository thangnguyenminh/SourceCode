#include "stdafx.h"
/*
ͬ���ͻ��˽��׺еı仯���Է�
*/

#include "CGExchangeSynchItemII.h"
#include "GCExchangeSynchII.h"
#include "GCExchangeError.h"
#include "GamePlayer.h"
#include "Obj_Human.h"
#include "Scene.h"
#include "Log.h"

#include "ItemOperator.h"
#include "HumanItemLogic.h"
#include "GCDetailAttrib_Pet.h"
#include "GameTable.h"
using namespace EXCHANGE_MSG;

uint CGExchangeSynchItemIIHandler::Execute( CGExchangeSynchItemII* pPacket, Player* pPlayer )
{
	__ENTER_FUNCTION

	GamePlayer* pGamePlayer = (GamePlayer*)pPlayer ;
	Assert( pGamePlayer ) ;

	Obj_Human* pHuman = pGamePlayer->GetHuman() ;
	Assert( pHuman ) ;

	Scene* pScene = pHuman->getScene() ;
	if( pScene==NULL )
	{
		Assert(FALSE) ;
		return PACKET_EXE_ERROR ;
	}

	//����߳�ִ����Դ�Ƿ���ȷ
	Assert( MyGetCurrentThreadID()==pScene->m_ThreadID ) ;

	//��֤����״̬
	EXCHANGE_CERTIFY_EACH_OTHER(pHuman)
	EXCHANGE_CERTIFY_ISLOCK(pHuman)
	ObjID_t	DestID = pHuman->m_ExchangBox.m_ObjID;
	Obj_Human* pDestHuman = pScene->GetHumanManager()->GetHuman( DestID );
	EXCHANGE_CERTIFY_STATUS(pHuman, EXCHANGE_SYNCH_DATA)
	EXCHANGE_CERTIFY_STATUS(pDestHuman, EXCHANGE_SYNCH_DATA)

	//����
	BYTE		Opt = pPacket->GetOpt();
	BYTE		FromType = pPacket->GetFromType();
	BYTE		ToType = pPacket->GetToType();
	BYTE		FromIndex = pPacket->GetFromIndex();
	BYTE		ToIndex = pPacket->GetToIndex();
	PET_GUID_t	PetGuid = pPacket->GetPetGuid();

	_ITEM				ItemTemp;
	GCExchangeSynchII	Msg;
	GCExchangeError		MsgError;
	ItemContainer*		pExchangeContainer		= &(pHuman->m_ExchangBox.m_Container);
	ItemContainer*		pExchangePetContainer	= &(pHuman->m_ExchangBox.m_PetContainer);

	switch(Opt)
	{
	case OPT_ERROR:
		{
		}
		break;
	case OPT_ADDITEM:
		{
			switch(FromType)
			{
			case POS_BAG:
				{
					//��֤λ�����Ƿ��ж���
					if(HumanItemLogic::GetItem(pHuman, FromIndex)->IsEmpty() == FALSE)
					{
						ItemContainer*	pBagContainer = HumanItemLogic::GetContainer(pHuman, FromIndex);

						//�Ȳ�һ�齻�׺����ǲ����Ѿ��и���Ʒ��
						for(INT i = 0; i<EXCHANGE_BOX_SIZE; i++)
						{
							_ITEM_GUID guid = (pBagContainer->GetItem(pBagContainer->BagIndex2ConIndex(FromIndex)))->GetGUID() ;
							if(pExchangeContainer->GetItem(i)->GetGUID() == guid)
							{//��Ʒ�Ѿ��ڽ��׺��У���������Ϣ
								return PACKET_EXE_CONTINUE ;
							}

						}

						//�Զ��Ҹ�
						INT result = 
							g_ItemOperator.CopyItem
							(
								pBagContainer,
								pBagContainer->BagIndex2ConIndex(FromIndex),
								pExchangeContainer
							);

						if(result>=0)
						{//�����ɹ���
							//������Ϣ��˫���ͻ���
							Item* pIt = pExchangeContainer->GetItem(result);
							g_pLog->FastSaveLog( LOG_FILE_1, "<����> [%s] ���뽻����Ʒ [%d]",	pHuman->GetName(), pIt->GetItemTableIndex() ) ;
							if(pIt->IsEmpty() == FALSE)
							{
								INT iConIndex = pBagContainer->BagIndex2ConIndex(FromIndex);
								//����������Ʒ
								g_ItemOperator.LockItem( pBagContainer, iConIndex );
								
								//��ʾ����Ʒ�Ѿ����ڽ�������,����ռ���
								pBagContainer->GetItem(iConIndex)->SetInExchange(TRUE);

								//�ٷ�
								pIt->SaveValueTo(&ItemTemp);//ȡ��ʵ��
								
								/*
								�ȷ��Լ�
								*/
								Msg.SetIsMyself(TRUE);//�����Լ�
								Msg.SetOpt(OPT_ADDITEM);//���ò�������
								Msg.SetFromType(POS_BAG);//������������
								Msg.SetFromIndex(FromIndex);//������������
								Msg.SetToIndex(result);//����Ŀ��λ������
								pGamePlayer->SendPacket(&Msg);

								/*
								�ٷ��Է�
								*/
								Msg.SetIsMyself(FALSE);//�����Է�
								Msg.SetOpt(OPT_ADDITEM);//���ò�������
								Msg.SetToIndex(result);
								_EXCHANGE_ITEM Exitem;
								Exitem.byNumber = ItemTemp.GetItemCount();
//								if( ItemTemp.ItemClass() == 1 && ItemTemp.GetQual() == 2 )
//								{//blue
									Exitem.isBlueEquip	= 1;
									Exitem.item_data	= ItemTemp;
//								}
//								else
//								{
//									Exitem.isBlueEquip	= 0;
//									Exitem.item_guid	= ItemTemp.m_ItemIndex;
//								}
								Msg.SetItem(&Exitem);
								pDestHuman->GetPlayer()->SendPacket(&Msg);
							}
							else
							{
								MsgError.SetID(ERR_ILLEGAL);
								pGamePlayer->SendPacket(&MsgError);
								g_pLog->FastSaveLog( LOG_FILE_1, "<����> [%s] ����Ƿ�������Ʒ [%d]",	pHuman->GetName(), pIt->GetItemTableIndex() ) ;
								return PACKET_EXE_CONTINUE ;
							}
						}
						else
						{//����ʧ��
							MsgError.SetID(ERR_NOT_ENOUGHT_EXROOM);
							pGamePlayer->SendPacket(&MsgError);
							g_pLog->FastSaveLog( LOG_FILE_1, "<����> [%s] ���׺�û���㹻�ռ�",	pHuman->GetName() ) ;
							return PACKET_EXE_CONTINUE ;
						}

					}

				}
				break;
			case POS_EQUIP:
				{
				}
				break;
			case POS_PET:
				{
				}
				break;
			default:
				break;
			}

		}
		break;
	case OPT_REMOVEITEM:
		{
			switch(ToType)
			{
				case POS_BAG:
					{
						if(pExchangeContainer->GetItem(FromIndex)->IsEmpty() == FALSE)
						{
							Item* pIt = pExchangeContainer->GetItem(FromIndex);
							ItemContainer*	pBagContainer = HumanItemLogic::GetItemContain( pHuman,pIt->GetItemTableIndex());
							INT BagIndex = pBagContainer->GetIndexByGUID( &pIt->GetGUID() );
							UINT	itemindex = pIt->GetItemTableIndex();
							
							//�ӽ��׺���ɾ������Ʒ
							if(g_ItemOperator.EraseItem(pExchangeContainer, FromIndex))
							{
								//��������Ʒ
								g_ItemOperator.UnlockItem( pBagContainer, BagIndex );

								//��ʾ����Ʒ�Ѿ����ڽ�������,����ռ���
								pBagContainer->GetItem(BagIndex)->SetInExchange(FALSE);

								//����Ϣ
								g_pLog->FastSaveLog( LOG_FILE_1, "<����> [%s] ɾ��������Ʒ [%d]",	pHuman->GetName(), itemindex) ;
								if(BagIndex>=0)
								{
									/*
									�ȷ��Լ�
									*/
									Msg.SetIsMyself(TRUE);//�����Լ�
									Msg.SetOpt(OPT_REMOVEITEM);//���ò�������
									Msg.SetToType(POS_BAG);//����Ŀ������
									Msg.SetToIndex(BagIndex);//����Ŀ������
									Msg.SetFromIndex(FromIndex);//������Դ����
									pGamePlayer->SendPacket(&Msg);

									/*
									�ٷ��Է�
									*/
									Msg.SetIsMyself(FALSE);//�����Է�
									Msg.SetOpt(OPT_REMOVEITEM);//���ò�������
									Msg.SetFromIndex(FromIndex);//������Դ����
									pDestHuman->GetPlayer()->SendPacket(&Msg);
								}
								else
								{
									MsgError.SetID(ERR_ILLEGAL);
									pGamePlayer->SendPacket(&MsgError);
									return PACKET_EXE_CONTINUE;
								}

							}
							else
							{
								MsgError.SetID(ERR_ILLEGAL);
								pGamePlayer->SendPacket(&MsgError);
								g_pLog->FastSaveLog( LOG_FILE_1, "<����> [%s] ɾ�����׺�λ�� [%d] ������Ʒʱ����",	pHuman->GetName(), FromIndex ) ;
								return PACKET_EXE_CONTINUE;
							}

						}
						else
						{
							MsgError.SetID(ERR_ILLEGAL);
							pGamePlayer->SendPacket(&MsgError);
							g_pLog->FastSaveLog( LOG_FILE_1, "<����> [%s] ���׺�λ�� [%d] ����������Ʒ",	pHuman->GetName(), FromIndex ) ;
							return PACKET_EXE_CONTINUE;
						}
					}
					break;

				case POS_EQUIP:
					{
					}
					break;

				case POS_PET:
					{
					}
					break;
				default:
					break;

			}

		}
		break;
	case OPT_ADDPET:
		{
			ItemContainer*	pPetContainer = pHuman->GetPetContain();
			FromIndex = pPetContainer->GetIndexByGUID( &PetGuid );

			//�Ȳ�һ�齻�׺����ǲ����Ѿ��и���Ʒ��
			for(INT i = 0; i<EXCHANGE_BOX_SIZE; i++)
			{
				if(pExchangePetContainer->GetItem(i)->GetPetGUID() == pPetContainer->GetItem(FromIndex)->GetPetGUID() )
				{//��Ʒ�Ѿ��ڽ��׺��У���������Ϣ
					return PACKET_EXE_CONTINUE ;
				}
			}

			//�Զ��Ҹ�
			INT result = 
				g_ItemOperator.CopyItem
				(
				pPetContainer,
				FromIndex,
				pExchangePetContainer
				);

			if(result>=0)
			{//�����ɹ�
				Item* pIt = pExchangePetContainer->GetItem(result);
				g_pLog->FastSaveLog( LOG_FILE_1, "<����> [%s] ���뽻�׳��� [%s]",	pHuman->GetName(), pIt->GetName() ) ;
				if(pIt->IsEmpty() == FALSE)
				{
					//����������Ʒ
					g_ItemOperator.LockItem( pPetContainer, FromIndex );

					//��ʾ����Ʒ�Ѿ����ڽ�������,����ռ���
					pPetContainer->GetItem(FromIndex)->SetInExchange(TRUE);
					/*
					�ȷ��Լ�
					*/
					Msg.SetIsMyself(TRUE);//�����Լ�
					Msg.SetOpt(OPT_ADDPET);//���ò�������
					Msg.SetToIndex(result);//����Ŀ�ĵ�λ��
					Msg.SetPetGuid(PetGuid);//����GUID
					pGamePlayer->SendPacket(&Msg);

					/*
					�ٷ��Է�
					*/
					GCDetailAttrib_Pet PetMsgDetail;
					//��װGCDetailAttrib_Pet�ṹ
					Obj_Human::CalculatePetDetailAttrib(PetMsgDetail, pIt);
					PetMsgDetail.SetTradeIndex( result );

					PET_EXTRA_STRUCT::GCExchangePetView_t ExtraPetInfo;
					ExtraPetInfo.m_bFlag	=	TYPE_EXCHANGE;
					PetMsgDetail.SetExtraInfoLength(ExtraPetInfo.GetSize());
					PetMsgDetail.SetExtraInfoData((BYTE*)&ExtraPetInfo);
					pDestHuman->GetPlayer()->SendPacket(&PetMsgDetail);	
				}
				else
				{
					MsgError.SetID(ERR_ILLEGAL);
					pGamePlayer->SendPacket(&MsgError);
					g_pLog->FastSaveLog( LOG_FILE_1, "<����> [%s] ����Ƿ����׳��� [%s]",	pHuman->GetName(), pIt->GetName() ) ;
					return PACKET_EXE_CONTINUE ;
				}
			}
		}
		break;
	case OPT_REMOVEPET:
		{
			ItemContainer*	pPetContainer = pHuman->GetPetContain();
			INT nIndexInPet = pPetContainer->GetIndexByGUID(&PetGuid);
			FromIndex = pExchangePetContainer->GetIndexByGUID(&PetGuid);

			if(g_ItemOperator.EraseItem(pExchangePetContainer, FromIndex))
			{
				//��������Ʒ
				g_ItemOperator.UnlockItem( pPetContainer, nIndexInPet );

				//��ʾ����Ʒ�Ѿ����ڽ�������,����ռ���
				pPetContainer->GetItem(nIndexInPet)->SetInExchange(FALSE);

				//����Ϣ
				g_pLog->FastSaveLog( LOG_FILE_1, "<����> [%s] ɾ�����׳��� [%s]",	pHuman->GetName(), pPetContainer->GetItem(nIndexInPet)->GetName()) ;
				if(nIndexInPet>=0)
				{
					/*
					�ȷ��Լ�
					*/
					Msg.SetIsMyself(TRUE);//�����Լ�
					Msg.SetOpt(OPT_REMOVEPET);//���ò�������
					Msg.SetPetGuid(PetGuid);//����GUID
					pGamePlayer->SendPacket(&Msg);

					/*
					�ٷ��Է�
					*/
					Msg.SetIsMyself(FALSE);//�����Է�
					Msg.SetOpt(OPT_REMOVEPET);//���ò�������
					Msg.SetPetGuid(PetGuid);//����GUID
					pDestHuman->GetPlayer()->SendPacket(&Msg);
				}
				else
				{
					MsgError.SetID(ERR_ILLEGAL);
					pGamePlayer->SendPacket(&MsgError);
					return PACKET_EXE_CONTINUE;
				}
			}
			else
			{
				MsgError.SetID(ERR_ILLEGAL);
				pGamePlayer->SendPacket(&MsgError);
				g_pLog->FastSaveLog( LOG_FILE_1, "<����> [%s] ɾ�����׺�λ�� [%d] ������Ʒʱ����",	pHuman->GetName(), FromIndex ) ;
				return PACKET_EXE_CONTINUE;
			}
		}
		break;
	default:
		break;
	}
		return PACKET_EXE_CONTINUE ;

	__LEAVE_FUNCTION

		return PACKET_EXE_ERROR ;
}