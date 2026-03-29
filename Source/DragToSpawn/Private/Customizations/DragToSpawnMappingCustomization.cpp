// Copyright 2026 Dream Seed LLC. All Rights Reserved.

#include "DragToSpawnMappingCustomization.h"
#include "DragToSpawnMapping.h"

#include "DetailWidgetRow.h"
#include "EditorClassUtils.h"
#include "IDetailChildrenBuilder.h"
#include "UObject/UnrealType.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SListView.h"
#include "DragToSpawnCompatibility.h"

#if UE_VERSION_NEWER_THAN_OR_EQUAL(5, 0, 0)
#include "Styling/AppStyle.h"
#else
#include "EditorStyleSet.h"
using FAppStyle = FEditorStyle;
#endif

TSharedRef<IPropertyTypeCustomization> FDragToSpawnMappingCustomization::MakeInstance()
{
	return MakeShareable(new FDragToSpawnMappingCustomization());
}

void FDragToSpawnMappingCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	AssetClassHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDragToSpawnMapping, AssetClass));
	ActorClassHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDragToSpawnMapping, ActorClassToSpawn));
	PropertyNameHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDragToSpawnMapping, PropertyName));

	FSimpleDelegate OnClassChanged = FSimpleDelegate::CreateSP(this, &FDragToSpawnMappingCustomization::RebuildPropertyNameOptions);
	AssetClassHandle->SetOnPropertyValueChanged(OnClassChanged);
	ActorClassHandle->SetOnPropertyValueChanged(OnClassChanged);

	RebuildPropertyNameOptions();

	const float Padding = 4.f;
	const bool bIsFirstElement = StructPropertyHandle->GetIndexInArray() == 0;

	TSharedRef<SHorizontalBox> WidgetsRow =
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(1.f)
		.Padding(0.f, 0.f, Padding, 0.f)
		[
			AssetClassHandle->CreatePropertyValueWidget(false)
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(0.f, 0.f, Padding, 0.f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT(" - - - > ")))
		]
		+ SHorizontalBox::Slot()
		.FillWidth(1.f)
		.Padding(0.f, 0.f, Padding, 0.f)
		[
			ActorClassHandle->CreatePropertyValueWidget(false)
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(0.f, 0.f, Padding, 0.f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT(".")))
		]
		+ SHorizontalBox::Slot()
		.FillWidth(0.6f)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::Get().GetBrush("ToolPanel.GroupBorder"))
			.Padding(1.f)
			[
				SAssignNew(PropertyNameComboButton, SComboButton)
				.ContentPadding(FMargin(0.f, 0.f, 4.f, 0.f))
				.ButtonStyle(FAppStyle::Get(), "NoBorder")
				.OnGetMenuContent_Lambda([this]() -> TSharedRef<SWidget>
				{
					return SNew(SBox)
						.MaxDesiredHeight(200.f)
						[
							SNew(SListView<TSharedPtr<FName>>)
							.ListItemsSource(&PropertyNameOptions)
							.OnGenerateRow_Lambda([](TSharedPtr<FName> Item, const TSharedRef<STableViewBase>& OwnerTable) -> TSharedRef<ITableRow>
							{
								return SNew(STableRow<TSharedPtr<FName>>, OwnerTable)
									[
										SNew(STextBlock)
										.Text(FText::FromName(*Item))
									];
							})
							.OnSelectionChanged_Lambda([this](TSharedPtr<FName> SelectedItem, ESelectInfo::Type SelectInfo)
							{
								if (SelectInfo != ESelectInfo::Direct && SelectedItem.IsValid())
								{
									OnPropertyNameSelected(SelectedItem);
								}
							})
						];
				})
				.ButtonContent()
				[
					SNew(SEditableTextBox)
					.Padding(FMargin(2.f, 2.f, -2.f, 2.f))
					.Text(this, &FDragToSpawnMappingCustomization::GetPropertyNameText)
					.OnTextCommitted(this, &FDragToSpawnMappingCustomization::OnPropertyNameTextCommitted)
					.Font(IPropertyTypeCustomizationUtils::GetRegularFont())
				]
			]
		];

	TSharedRef<SWidget> ValueWidget = bIsFirstElement
		? StaticCastSharedRef<SWidget>(
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 4.f, 0.f, 2.f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(1.f)
				.VAlign(VAlign_Center)
				.Padding(0.f, 0.f, Padding, 0.f)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Draggable Type")))
					.Font(IPropertyTypeCustomizationUtils::GetBoldFont())
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0.f, 0.f, Padding, 0.f)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT(" - - - > ")))
					.ColorAndOpacity(FSlateColor(FLinearColor::Transparent))
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1.f)
				.VAlign(VAlign_Center)
				.Padding(0.f, 0.f, Padding, 0.f)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Spawn Actor")))
					.Font(IPropertyTypeCustomizationUtils::GetBoldFont())
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0.f, 0.f, Padding, 0.f)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT(".")))
					.ColorAndOpacity(FSlateColor(FLinearColor::Transparent))
				]
				+ SHorizontalBox::Slot()
				.FillWidth(0.6f)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Set Property")))
					.Font(IPropertyTypeCustomizationUtils::GetBoldFont())
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 0.f, 0.f, 4.f)
			[
				SNew(SSeparator)
				.Orientation(Orient_Horizontal)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				WidgetsRow
			]
		)
		: StaticCastSharedRef<SWidget>(WidgetsRow);

	HeaderRow
		.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MinDesiredWidth(1200.f)
		[
			ValueWidget
		];
}

void FDragToSpawnMappingCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void FDragToSpawnMappingCustomization::RebuildPropertyNameOptions()
{
	PropertyNameOptions.Reset();

	if (!ActorClassHandle || !AssetClassHandle)
	{
		return;
	}

	FString ActorClassPath;
	ActorClassHandle->GetValueAsFormattedString(ActorClassPath);
	UClass* ActorClass = FEditorClassUtils::GetClassFromString(ActorClassPath);

	FString AssetClassPath;
	AssetClassHandle->GetValueAsFormattedString(AssetClassPath);
	UClass* AssetClass = FEditorClassUtils::GetClassFromString(AssetClassPath);

	if (!ActorClass)
	{
		return;
	}

	for (TFieldIterator<FProperty> PropertyIterator(ActorClass); PropertyIterator; ++PropertyIterator)
	{
		FProperty* Property = *PropertyIterator;
		if (!Property->HasAnyPropertyFlags(CPF_Edit | CPF_BlueprintVisible))
		{
			continue;
		}

		UClass* ConstraintClass = nullptr;

		// Check derived types before base types (FClassProperty extends FObjectProperty, FSoftClassProperty extends FSoftObjectProperty).
		if (const FClassProperty* ClassProperty = CastField<FClassProperty>(Property))
		{
			ConstraintClass = ClassProperty->MetaClass;
		}
		else if (const FSoftClassProperty* SoftClassProperty = CastField<FSoftClassProperty>(Property))
		{
			ConstraintClass = SoftClassProperty->MetaClass;
		}
		else if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
		{
			ConstraintClass = ObjectProperty->PropertyClass;
		}
		else if (const FSoftObjectProperty* SoftObjectProperty = CastField<FSoftObjectProperty>(Property))
		{
			ConstraintClass = SoftObjectProperty->PropertyClass;
		}
		else
		{
			continue;
		}

		if (!ConstraintClass)
		{
			continue;
		}

		// Filter down to compatible property types of the AssetClass if it is set.
		if (AssetClass && !AssetClass->IsChildOf(ConstraintClass))
		{
			continue;
		}

		PropertyNameOptions.Add(MakeShared<FName>(Property->GetFName()));
	}

	PropertyNameOptions.Sort([](const TSharedPtr<FName>& A, const TSharedPtr<FName>& B)
	{
		return A->LexicalLess(*B);
	});
}

void FDragToSpawnMappingCustomization::OnPropertyNameSelected(TSharedPtr<FName> PropertyName)
{
	if (PropertyNameHandle && PropertyName.IsValid())
	{
		PropertyNameHandle->SetValue(*PropertyName);
	}
	if (PropertyNameComboButton)
	{
		PropertyNameComboButton->SetIsOpen(false);
	}
}

FText FDragToSpawnMappingCustomization::GetPropertyNameText() const
{
	if (!PropertyNameHandle)
	{
		return FText::GetEmpty();
	}

	FName Value;
	PropertyNameHandle->GetValue(Value);
	return FText::FromName(Value);
}

void FDragToSpawnMappingCustomization::OnPropertyNameTextCommitted(const FText& NewText, ETextCommit::Type CommitType)
{
	if (PropertyNameHandle)
	{
		PropertyNameHandle->SetValue(FName(*NewText.ToString()));
	}
}
